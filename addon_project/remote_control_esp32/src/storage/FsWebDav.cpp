#include "FsWebDav.h"

#include <esp_arduino_version.h>
#include <time.h>
#include <vector>

// File system servito e spazio (impostati da fsWebDavBegin)
static fs::FS *davFs = nullptr;
static FsWebDavBytesFn davTotalBytes = nullptr;
static FsWebDavBytesFn davUsedBytes = nullptr;
#define DAVFS (*davFs)

// Data usata quando il file non ha un orario valido (niente RTC/NTP: FAT scrive 1980)
static const time_t FALLBACK_TIME = 1735689600; // 2025-01-01 00:00:00 UTC

// ---------------------------------------------------------------------------
// Percorsi e codifiche
// ---------------------------------------------------------------------------

static int hexValue(char c)
{
  if (c >= '0' && c <= '9')
    return c - '0';
  if (c >= 'a' && c <= 'f')
    return c - 'a' + 10;
  if (c >= 'A' && c <= 'F')
    return c - 'A' + 10;
  return -1;
}

// "%20" -> " " (il '+' resta '+': nei percorsi non è uno spazio)
static String urlDecode(const String &in)
{
  String out;
  out.reserve(in.length());
  for (size_t i = 0; i < in.length(); i++)
  {
    char c = in[i];
    if (c == '%' && i + 2 < in.length() && hexValue(in[i + 1]) >= 0 && hexValue(in[i + 2]) >= 0)
    {
      out += (char)(hexValue(in[i + 1]) * 16 + hexValue(in[i + 2]));
      i += 2;
    }
    else
    {
      out += c;
    }
  }
  return out;
}

// Codifica per gli href: tutto tranne caratteri non riservati e '/'
static String urlEncode(const String &in)
{
  static const char *hex = "0123456789ABCDEF";
  String out;
  for (size_t i = 0; i < in.length(); i++)
  {
    uint8_t c = (uint8_t)in[i];
    if (isalnum(c) || c == '/' || c == '-' || c == '_' || c == '.' || c == '~')
    {
      out += (char)c;
    }
    else
    {
      out += '%';
      out += hex[c >> 4];
      out += hex[c & 0x0F];
    }
  }
  return out;
}

static String xmlEscape(const String &in)
{
  String out;
  for (size_t i = 0; i < in.length(); i++)
  {
    char c = in[i];
    if (c == '&')
      out += "&amp;";
    else if (c == '<')
      out += "&lt;";
    else if (c == '>')
      out += "&gt;";
    else if (c == '"')
      out += "&quot;";
    else
      out += c;
  }
  return out;
}

// URI della richiesta -> percorso sul file system ("/config/a.json"; radice "/").
// false se il percorso non è accettabile (es. contiene "..").
static bool uriToPath(const String &uri, String &path)
{
  path = urlDecode(uri);
  if (!path.startsWith("/"))
    path = "/" + path;
  while (path.length() > 1 && path.endsWith("/"))
    path.remove(path.length() - 1);
  return path.indexOf("/../") < 0 && !path.endsWith("/..");
}

// Header Destination ("http://host/percorso") -> percorso sul file system
static bool destinationToPath(const String &destination, String &path)
{
  String uri = destination;
  int scheme = uri.indexOf("://");
  if (scheme >= 0)
  {
    int slash = uri.indexOf('/', scheme + 3);
    uri = slash >= 0 ? uri.substring(slash) : "/";
  }
  return uriToPath(uri, path);
}

static String parentOf(const String &path)
{
  int slash = path.lastIndexOf('/');
  return slash <= 0 ? "/" : path.substring(0, slash);
}

static String nameOf(const String &path)
{
  int slash = path.lastIndexOf('/');
  return path.substring(slash + 1);
}

static bool isDirectory(const String &path)
{
  if (path != "/" && !DAVFS.exists(path))
    return false;
  File f = DAVFS.open(path);
  if (!f)
    return false;
  bool dir = f.isDirectory();
  f.close();
  return dir;
}

static String httpDate(time_t t)
{
  if (t < 946684800) // prima del 2000: orario non valido
    t = FALLBACK_TIME;
  struct tm tmv;
  gmtime_r(&t, &tmv);
  char buf[40];
  strftime(buf, sizeof(buf), "%a, %d %b %Y %H:%M:%S GMT", &tmv);
  return buf;
}

static String isoDate(time_t t)
{
  if (t < 946684800)
    t = FALLBACK_TIME;
  struct tm tmv;
  gmtime_r(&t, &tmv);
  char buf[32];
  strftime(buf, sizeof(buf), "%Y-%m-%dT%H:%M:%SZ", &tmv);
  return buf;
}

static String contentTypeFor(const String &path)
{
  String p = path;
  p.toLowerCase();
  if (p.endsWith(".json"))
    return "application/json";
  if (p.endsWith(".txt") || p.endsWith(".ini") || p.endsWith(".csv") || p.endsWith(".log"))
    return "text/plain";
  if (p.endsWith(".html") || p.endsWith(".htm"))
    return "text/html";
  if (p.endsWith(".png"))
    return "image/png";
  if (p.endsWith(".jpg") || p.endsWith(".jpeg"))
    return "image/jpeg";
  return "application/octet-stream";
}

// Cartelle di sistema create da Windows sulle schede inserite nel PC: restano sul file system ma non
// compaiono negli elenchi della risorsa di rete
static bool isHiddenEntry(const String &name)
{
  return name.equalsIgnoreCase("System Volume Information");
}

// ---------------------------------------------------------------------------
// Operazioni ricorsive
// ---------------------------------------------------------------------------

static std::vector<String> listChildren(const String &dirPath)
{
  std::vector<String> children;
  if (dirPath != "/" && !DAVFS.exists(dirPath))
    return children;
  File dir = DAVFS.open(dirPath);
  if (!dir || !dir.isDirectory())
    return children;
  File child = dir.openNextFile();
  while (child)
  {
    String childPath = (dirPath == "/" ? "" : dirPath) + "/" + child.name();
    children.push_back(childPath);
    child.close();
    child = dir.openNextFile();
  }
  dir.close();
  return children;
}

static bool removeRecursive(const String &path)
{
  if (!isDirectory(path))
    return DAVFS.remove(path);
  for (const String &child : listChildren(path))
  {
    if (!removeRecursive(child))
      return false;
  }
  return DAVFS.rmdir(path);
}

static bool copyFile(const String &from, const String &to)
{
  if (!DAVFS.exists(from))
    return false;
  File in = DAVFS.open(from, FILE_READ);
  if (!in)
    return false;
  File out = DAVFS.open(to, FILE_WRITE);
  if (!out)
  {
    in.close();
    return false;
  }
  uint8_t buf[512];
  bool ok = true;
  while (in.available())
  {
    size_t n = in.read(buf, sizeof(buf));
    if (out.write(buf, n) != n)
    {
      ok = false;
      break;
    }
  }
  in.close();
  out.close();
  return ok;
}

static bool copyRecursive(const String &from, const String &to)
{
  if (!isDirectory(from))
    return copyFile(from, to);
  if (!DAVFS.exists(to) && !DAVFS.mkdir(to))
    return false;
  for (const String &child : listChildren(from))
  {
    if (!copyRecursive(child, to + "/" + nameOf(child)))
      return false;
  }
  return true;
}

// ---------------------------------------------------------------------------
// Risposte
// ---------------------------------------------------------------------------

static void sendDavHeaders(WebServer &server)
{
  server.sendHeader("DAV", "1, 2");
  server.sendHeader("MS-Author-Via", "DAV");
}

static void sendEmpty(WebServer &server, int code)
{
  sendDavHeaders(server);
  server.send(code, "text/plain", "");
}

// Un <D:response> di PROPFIND per un file o una cartella
static String propfindEntry(File &f, const String &path, bool withQuota)
{
  bool dir = f.isDirectory();
  String href = urlEncode(path);
  if (dir && !href.endsWith("/"))
    href += "/";
  String name = path == "/" ? "" : nameOf(path);
  time_t t = f.getLastWrite();

  String xml;
  xml.reserve(512);
  xml += "<D:response><D:href>" + href + "</D:href><D:propstat><D:prop>";
  xml += "<D:displayname>" + xmlEscape(name) + "</D:displayname>";
  xml += "<D:creationdate>" + isoDate(t) + "</D:creationdate>";
  xml += "<D:getlastmodified>" + httpDate(t) + "</D:getlastmodified>";
  if (dir)
  {
    xml += "<D:resourcetype><D:collection/></D:resourcetype>";
    if (withQuota && davTotalBytes && davUsedBytes)
    {
      uint64_t total = davTotalBytes();
      uint64_t used = davUsedBytes();
      xml += "<D:quota-available-bytes>" + String((unsigned long long)(total - used)) + "</D:quota-available-bytes>";
      xml += "<D:quota-used-bytes>" + String((unsigned long long)used) + "</D:quota-used-bytes>";
    }
  }
  else
  {
    xml += "<D:resourcetype/>";
    xml += "<D:getcontentlength>" + String((unsigned long)f.size()) + "</D:getcontentlength>";
    xml += "<D:getcontenttype>" + contentTypeFor(path) + "</D:getcontenttype>";
  }
  xml += "</D:prop><D:status>HTTP/1.1 200 OK</D:status></D:propstat></D:response>";
  return xml;
}

static void handlePropfind(WebServer &server, const String &path)
{
  File f;
  if (path == "/" || DAVFS.exists(path))
    f = DAVFS.open(path);
  if (!f)
  {
    sendEmpty(server, 404);
    return;
  }

  // Depth: 0 = solo l'elemento, 1 (o assente/infinity) = anche il contenuto della cartella
  bool depthZero = server.header("Depth") == "0";

  sendDavHeaders(server);
  server.setContentLength(CONTENT_LENGTH_UNKNOWN); // risposta a pezzi: cartelle grandi senza esaurire la RAM
  server.send(207, "application/xml; charset=utf-8", "");
  server.sendContent("<?xml version=\"1.0\" encoding=\"utf-8\"?><D:multistatus xmlns:D=\"DAV:\">");
  server.sendContent(propfindEntry(f, path, path == "/"));

  if (f.isDirectory() && !depthZero)
  {
    File child = f.openNextFile();
    while (child)
    {
      String childPath = (path == "/" ? "" : path) + "/" + child.name();
      if (!isHiddenEntry(child.name()))
        server.sendContent(propfindEntry(child, childPath, false));
      child.close();
      child = f.openNextFile();
    }
  }
  f.close();
  server.sendContent("</D:multistatus>");
  server.sendContent(""); // fine della risposta a pezzi
}

static void handleGet(WebServer &server, const String &path, bool headOnly)
{
  File f;
  if (path == "/" || DAVFS.exists(path))
    f = DAVFS.open(path, FILE_READ);
  if (!f)
  {
    sendEmpty(server, 404);
    return;
  }

  if (f.isDirectory())
  {
    // dal browser: elenco semplice dei file
    String html = "<html><body><h3>" + xmlEscape(path) + "</h3><ul>";
    File child = f.openNextFile();
    while (child)
    {
      String childPath = (path == "/" ? "" : path) + "/" + child.name();
      if (!isHiddenEntry(child.name()))
        html += "<li><a href=\"" + urlEncode(childPath) + (child.isDirectory() ? "/" : "") + "\">" +
              xmlEscape(child.name()) + (child.isDirectory() ? "/" : "") + "</a></li>";
      child.close();
      child = f.openNextFile();
    }
    f.close();
    html += "</ul></body></html>";
    server.send(200, "text/html; charset=utf-8", headOnly ? "" : html);
    return;
  }

  server.sendHeader("Last-Modified", httpDate(f.getLastWrite()));
  if (headOnly)
  {
    server.setContentLength(f.size());
    server.send(200, contentTypeFor(path), "");
  }
  else
  {
    server.streamFile(f, contentTypeFor(path));
  }
  f.close();
}

static void handleDelete(WebServer &server, const String &path)
{
  if (path == "/")
  {
    sendEmpty(server, 403);
    return;
  }
  if (!DAVFS.exists(path))
  {
    sendEmpty(server, 404);
    return;
  }
  sendEmpty(server, removeRecursive(path) ? 204 : 500);
}

static void handleMkcol(WebServer &server, const String &path)
{
  if (DAVFS.exists(path))
  {
    sendEmpty(server, 405);
    return;
  }
  if (!isDirectory(parentOf(path)))
  {
    sendEmpty(server, 409);
    return;
  }
  sendEmpty(server, DAVFS.mkdir(path) ? 201 : 500);
}

static void handleMoveCopy(WebServer &server, const String &path, bool move)
{
  String destination;
  if (!destinationToPath(server.header("Destination"), destination) || destination == "/" || path == "/")
  {
    sendEmpty(server, 400);
    return;
  }
  if (!DAVFS.exists(path))
  {
    sendEmpty(server, 404);
    return;
  }
  if (!isDirectory(parentOf(destination)))
  {
    sendEmpty(server, 409);
    return;
  }

  bool existed = DAVFS.exists(destination);
  if (existed)
  {
    if (server.header("Overwrite") == "F")
    {
      sendEmpty(server, 412);
      return;
    }
    if (!removeRecursive(destination))
    {
      sendEmpty(server, 500);
      return;
    }
  }

  bool ok = move ? DAVFS.rename(path, destination) : copyRecursive(path, destination);
  sendEmpty(server, ok ? (existed ? 204 : 201) : 500);
}

// LOCK finto: Windows lo richiede prima di scrivere. Se il file non esiste lo crea vuoto.
static void handleLock(WebServer &server, const String &uri, const String &path)
{
  int code = 200;
  if (!DAVFS.exists(path))
  {
    File f = DAVFS.open(path, FILE_WRITE);
    if (!f)
    {
      sendEmpty(server, 409);
      return;
    }
    f.close();
    code = 201;
  }

  char token[48];
  snprintf(token, sizeof(token), "opaquelocktoken:%08lx-%08lx", (unsigned long)esp_random(), (unsigned long)esp_random());
  String body = "<?xml version=\"1.0\" encoding=\"utf-8\"?><D:prop xmlns:D=\"DAV:\"><D:lockdiscovery><D:activelock>"
                "<D:locktype><D:write/></D:locktype><D:lockscope><D:exclusive/></D:lockscope>"
                "<D:depth>infinity</D:depth><D:timeout>Second-3600</D:timeout>"
                "<D:locktoken><D:href>" +
                String(token) + "</D:href></D:locktoken><D:lockroot><D:href>" + xmlEscape(uri) +
                "</D:href></D:lockroot></D:activelock></D:lockdiscovery></D:prop>";

  sendDavHeaders(server);
  server.sendHeader("Lock-Token", "<" + String(token) + ">");
  server.send(code, "application/xml; charset=utf-8", body);
}

// PROPPATCH: Windows prova a impostare le date dei file; si risponde "fatto" senza modificare nulla
static void handleProppatch(WebServer &server, const String &uri, const String &path)
{
  if (!DAVFS.exists(path))
  {
    sendEmpty(server, 404);
    return;
  }
  String body = "<?xml version=\"1.0\" encoding=\"utf-8\"?><D:multistatus xmlns:D=\"DAV:\"><D:response><D:href>" +
                xmlEscape(uri) + "</D:href><D:propstat><D:prop/><D:status>HTTP/1.1 200 OK</D:status></D:propstat>"
                                 "</D:response></D:multistatus>";
  sendDavHeaders(server);
  server.send(207, "application/xml; charset=utf-8", body);
}

// ---------------------------------------------------------------------------
// Gestore WebServer
// ---------------------------------------------------------------------------

class WebDavHandler : public RequestHandler
{
public:
  explicit WebDavHandler(WebServer &server) : server(server) {}

  // Le firme dei metodi virtuali di RequestHandler cambiano tra core 2.x e 3.x
#if ESP_ARDUINO_VERSION_MAJOR >= 3
  bool canHandle(WebServer &, HTTPMethod, const String &) override { return true; }
  bool canRaw(WebServer &, const String &) override { return doCanRaw(); }
  void raw(WebServer &, const String &uri, HTTPRaw &raw) override { doRaw(uri, raw); }
  bool handle(WebServer &, HTTPMethod method, const String &uri) override { return doHandle(method, uri); }
#else
  bool canHandle(HTTPMethod, String) override { return true; }
  bool canRaw(String) override { return doCanRaw(); }
  void raw(WebServer &, String uri, HTTPRaw &raw) override { doRaw(uri, raw); }
  bool handle(WebServer &, HTTPMethod method, String uri) override { return doHandle(method, uri); }
#endif

private:
  WebServer &server;
  File putFile;
  String putPath;
  bool putOk = false;
  bool putExisted = false;

  // il corpo dei PUT arriva a pezzi in raw(): file grandi senza caricarli in RAM
  bool doCanRaw()
  {
    return server.method() == HTTP_PUT;
  }

  void doRaw(const String &uri, HTTPRaw &raw)
  {
    switch (raw.status)
    {
    case RAW_START:
    {
      putOk = false;
      putExisted = false;
      if (!uriToPath(uri, putPath) || putPath == "/" || !isDirectory(parentOf(putPath)) || isDirectory(putPath))
        return;
      putExisted = DAVFS.exists(putPath);
      putFile = DAVFS.open(putPath, FILE_WRITE); // crea o svuota
      putOk = (bool)putFile;
      break;
    }
    case RAW_WRITE:
      if (putOk && putFile.write(raw.buf, raw.currentSize) != raw.currentSize)
        putOk = false;
      break;
    case RAW_END:
      if (putFile)
        putFile.close();
      break;
    case RAW_ABORTED:
      if (putFile)
        putFile.close();
      if (putOk)
        DAVFS.remove(putPath); // trasferimento interrotto: niente file a metà
      putOk = false;
      break;
    }
  }

  bool doHandle(HTTPMethod method, const String &uri)
  {
    String path;
    if (!uriToPath(uri, path))
    {
      sendEmpty(server, 403);
      return true;
    }

    switch (method)
    {
    case HTTP_OPTIONS:
      sendDavHeaders(server);
      server.sendHeader("Allow", "OPTIONS, GET, HEAD, PUT, DELETE, PROPFIND, PROPPATCH, MKCOL, MOVE, COPY, LOCK, UNLOCK");
      server.send(200, "text/plain", "");
      break;
    case HTTP_PROPFIND:
      handlePropfind(server, path);
      break;
    case HTTP_GET:
      handleGet(server, path, false);
      break;
    case HTTP_HEAD:
      handleGet(server, path, true);
      break;
    case HTTP_PUT:
      // il file è già stato scritto in raw()
      if (server.clientContentLength() == 0 && !putOk)
      {
        // PUT vuoto (Windows crea così i file nuovi): raw() non viene chiamato
        File f;
        if (path != "/" && isDirectory(parentOf(path)) && !isDirectory(path))
        {
          putExisted = DAVFS.exists(path);
          f = DAVFS.open(path, FILE_WRITE);
        }
        putOk = (bool)f;
        if (f)
          f.close();
      }
      sendEmpty(server, putOk ? (putExisted ? 204 : 201) : 409);
      putOk = false;
      break;
    case HTTP_DELETE:
      handleDelete(server, path);
      break;
    case HTTP_MKCOL:
      handleMkcol(server, path);
      break;
    case HTTP_MOVE:
      handleMoveCopy(server, path, true);
      break;
    case HTTP_COPY:
      handleMoveCopy(server, path, false);
      break;
    case HTTP_LOCK:
      handleLock(server, uri, path);
      break;
    case HTTP_UNLOCK:
      sendEmpty(server, 204);
      break;
    case HTTP_PROPPATCH:
      handleProppatch(server, uri, path);
      break;
    default:
      sendEmpty(server, 405);
      break;
    }
    return true;
  }
};

void fsWebDavBegin(WebServer &server, fs::FS &fs, FsWebDavBytesFn totalBytes, FsWebDavBytesFn usedBytes)
{
  davFs = &fs;
  davTotalBytes = totalBytes;
  davUsedBytes = usedBytes;
  static const char *headers[] = {"Depth", "Destination", "Overwrite"};
  server.collectHeaders(headers, 3);
  server.addHandler(new WebDavHandler(server));
}
