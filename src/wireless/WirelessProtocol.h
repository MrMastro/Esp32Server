#ifndef WIRELESS_PROTOCOL_H
#define WIRELESS_PROTOCOL_H

// Joystick wireless telecomando <-> dispositivi (addon_project/remote_control_esp32/docs/WIRELESS_JOYSTICK.md).
// File IDENTICO nei due progetti:
//   src/wireless/WirelessProtocol.h                              (Esp32Server, core 2.x / ESP-IDF 4.4)
//   addon_project/remote_control_esp32/src/wireless/WirelessProtocol.h (telecomando, core 3.x / ESP-IDF 5.x)
// Se lo modifichi, copialo nell'altro progetto.

#include <Arduino.h>
#include <mbedtls/md.h>
#if __has_include(<esp_random.h>)
#include <esp_random.h> // ESP-IDF 5.x
#else
#include <esp_system.h> // ESP-IDF 4.4: esp_fill_random
#endif

#define WIRELESS_PROTOCOL_VERSION 1
#define WIRELESS_NONCE_LEN 16       // byte casuali di ciascun lato (32 caratteri hex nel JSON)
#define WIRELESS_SESSION_KEY_LEN 32 // HMAC-SHA256
#define WIRELESS_MAC_LEN 6
#define WIRELESS_PAIR_PATH "/api/v1/remote/pair"

// "AA:BB:CC:DD:EE:FF" -> 6 byte
inline bool wirelessParseMac(const String &text, uint8_t mac[WIRELESS_MAC_LEN])
{
  unsigned int v[WIRELESS_MAC_LEN];
  if (sscanf(text.c_str(), "%x:%x:%x:%x:%x:%x", &v[0], &v[1], &v[2], &v[3], &v[4], &v[5]) != WIRELESS_MAC_LEN)
    return false;
  for (int i = 0; i < WIRELESS_MAC_LEN; i++)
  {
    if (v[i] > 0xFF)
      return false;
    mac[i] = (uint8_t)v[i];
  }
  return true;
}

// 6 byte -> "AA:BB:CC:DD:EE:FF"
inline String wirelessMacToString(const uint8_t mac[WIRELESS_MAC_LEN])
{
  char buf[18];
  snprintf(buf, sizeof(buf), "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  return String(buf);
}

inline String wirelessToHex(const uint8_t *data, size_t len)
{
  static const char digits[] = "0123456789abcdef";
  String out;
  out.reserve(len * 2);
  for (size_t i = 0; i < len; i++)
  {
    out += digits[data[i] >> 4];
    out += digits[data[i] & 0x0F];
  }
  return out;
}

// Hex (maiuscolo o minuscolo) -> esattamente len byte
inline bool wirelessFromHex(const String &hex, uint8_t *out, size_t len)
{
  if (hex.length() != len * 2)
    return false;
  for (size_t i = 0; i < len; i++)
  {
    char hi = hex[i * 2], lo = hex[i * 2 + 1];
    if (!isHexadecimalDigit(hi) || !isHexadecimalDigit(lo))
      return false;
    char pair[3] = {hi, lo, 0};
    out[i] = (uint8_t)strtoul(pair, nullptr, 16);
  }
  return true;
}

// Byte casuali dal generatore hardware (casuale vero con la radio WiFi accesa)
inline void wirelessRandomBytes(uint8_t *out, size_t len)
{
  esp_fill_random(out, len);
}

// sessionKey = HMAC-SHA256(devicePassword, remoteNonce || deviceNonce || remoteMac || deviceMac)
// Nonce e MAC in binario (16 + 16 + 6 + 6 = 44 byte). La password non viaggia piu' dopo l'accoppiamento.
inline bool wirelessDeriveSessionKey(const String &devicePassword,
                                     const uint8_t remoteNonce[WIRELESS_NONCE_LEN],
                                     const uint8_t deviceNonce[WIRELESS_NONCE_LEN],
                                     const uint8_t remoteMac[WIRELESS_MAC_LEN],
                                     const uint8_t deviceMac[WIRELESS_MAC_LEN],
                                     uint8_t sessionKey[WIRELESS_SESSION_KEY_LEN])
{
  uint8_t input[WIRELESS_NONCE_LEN * 2 + WIRELESS_MAC_LEN * 2];
  memcpy(input, remoteNonce, WIRELESS_NONCE_LEN);
  memcpy(input + WIRELESS_NONCE_LEN, deviceNonce, WIRELESS_NONCE_LEN);
  memcpy(input + WIRELESS_NONCE_LEN * 2, remoteMac, WIRELESS_MAC_LEN);
  memcpy(input + WIRELESS_NONCE_LEN * 2 + WIRELESS_MAC_LEN, deviceMac, WIRELESS_MAC_LEN);
  const mbedtls_md_info_t *md = mbedtls_md_info_from_type(MBEDTLS_MD_SHA256);
  return md != nullptr &&
         mbedtls_md_hmac(md, (const unsigned char *)devicePassword.c_str(), devicePassword.length(),
                         input, sizeof(input), sessionKey) == 0;
}

// ---------------------------------------------------------------------------
// Pacchetti ESP-NOW (fase 2+): 15 byte, firmati con la chiave di sessione.
// Interi little-endian (entrambi gli ESP32 lo sono: la struct si invia cosi' com'e').
// ---------------------------------------------------------------------------

enum : uint8_t
{
  WIRELESS_PKT_SESSION_START = 1,   // tel -> disp: esci dal modem sleep + applica direction (fase 4)
  WIRELESS_PKT_DIRECTION = 2,       // tel -> disp: nuova direzione
  WIRELESS_PKT_HEARTBEAT = 3,       // tel -> disp: sono vivo + direzione attuale (fase 3)
  WIRELESS_PKT_SESSION_END = 4,     // tel -> disp: torna in modem sleep (fase 4)
  WIRELESS_PKT_HEARTBEAT_ACK = 5,   // disp -> tel: heartbeat accettato (fase 3)
  WIRELESS_PKT_UNKNOWN_SESSION = 6, // disp -> tel: firma non valida / sessione sconosciuta (fase 3)
  WIRELESS_PKT_REMOTE_PAUSE = 7,    // tel -> disp: telecomando sospeso (fase 5)
  WIRELESS_PKT_REMOTE_PAUSE_ACK = 8 // disp -> tel: REMOTE_PAUSE accettato (fase 5)
};

#define WIRELESS_HMAC_LEN 8 // primi 8 byte di HMAC-SHA256
#define WIRELESS_DIRECTION_NONE 255

struct __attribute__((packed)) WirelessPacket
{
  uint8_t version;   // WIRELESS_PROTOCOL_VERSION
  uint8_t type;      // WIRELESS_PKT_*
  uint32_t seq;      // crescente per accoppiamento (parte da 1): il dispositivo accetta solo seq > ultimo (anti-replay)
  uint8_t direction; // codice direzione (tabella sotto), WIRELESS_DIRECTION_NONE se non serve
  uint8_t hmac[WIRELESS_HMAC_LEN]; // HMAC-SHA256(sessionKey, version..direction), primi 8 byte
};

static_assert(sizeof(WirelessPacket) == 15, "WirelessPacket deve essere di 15 byte");

// Byte firmati: tutto tranne l'hmac
#define WIRELESS_SIGNED_LEN (sizeof(WirelessPacket) - WIRELESS_HMAC_LEN)

inline bool wirelessComputeHmac(const uint8_t sessionKey[WIRELESS_SESSION_KEY_LEN], const WirelessPacket &packet,
                                uint8_t out[WIRELESS_HMAC_LEN])
{
  uint8_t full[32];
  const mbedtls_md_info_t *md = mbedtls_md_info_from_type(MBEDTLS_MD_SHA256);
  if (md == nullptr || mbedtls_md_hmac(md, sessionKey, WIRELESS_SESSION_KEY_LEN,
                                       (const unsigned char *)&packet, WIRELESS_SIGNED_LEN, full) != 0)
    return false;
  memcpy(out, full, WIRELESS_HMAC_LEN);
  return true;
}

// Riempie version, type, seq, direction e firma il pacchetto
inline bool wirelessBuildPacket(const uint8_t sessionKey[WIRELESS_SESSION_KEY_LEN], uint8_t type, uint32_t seq,
                                uint8_t direction, WirelessPacket &packet)
{
  packet.version = WIRELESS_PROTOCOL_VERSION;
  packet.type = type;
  packet.seq = seq;
  packet.direction = direction;
  return wirelessComputeHmac(sessionKey, packet, packet.hmac);
}

// true se la firma e' valida (confronto a tempo costante)
inline bool wirelessVerifyPacket(const uint8_t sessionKey[WIRELESS_SESSION_KEY_LEN], const WirelessPacket &packet)
{
  uint8_t expected[WIRELESS_HMAC_LEN];
  if (!wirelessComputeHmac(sessionKey, packet, expected))
    return false;
  uint8_t diff = 0;
  for (int i = 0; i < WIRELESS_HMAC_LEN; i++)
    diff |= expected[i] ^ packet.hmac[i];
  return diff == 0;
}

// Codici direzione FISSI (non dipendono dall'ordine degli enum J_DIRECTION dei due progetti):
// il codice e' l'indice in questa tabella; 255 = nessuna direzione.
static const char *const WIRELESS_DIRECTION_NAMES[] = {
    "CENTER", "UP", "DOWN", "LEFT", "RIGHT", "UP_LEFT", "UP_RIGHT", "DOWN_LEFT", "DOWN_RIGHT",                                         // 0-8
    "CENTER_BUTTON", "UP_BUTTON", "DOWN_BUTTON", "LEFT_BUTTON", "RIGHT_BUTTON", "UP_LEFT_BUTTON", "UP_RIGHT_BUTTON", "DOWN_LEFT_BUTTON", // 9-16
    "DOWN_RIGHT_BUTTON"                                                                                                                 // 17
};
#define WIRELESS_DIRECTION_COUNT (sizeof(WIRELESS_DIRECTION_NAMES) / sizeof(WIRELESS_DIRECTION_NAMES[0]))

// Codice -> stringa del comando ("UP_BUTTON"); "" se il codice non e' valido
inline String wirelessDirectionToString(uint8_t code)
{
  return code < WIRELESS_DIRECTION_COUNT ? String(WIRELESS_DIRECTION_NAMES[code]) : String("");
}

// Stringa -> codice; WIRELESS_DIRECTION_NONE se sconosciuta
inline uint8_t wirelessDirectionFromString(const String &name)
{
  for (uint8_t i = 0; i < WIRELESS_DIRECTION_COUNT; i++)
  {
    if (name == WIRELESS_DIRECTION_NAMES[i])
      return i;
  }
  return WIRELESS_DIRECTION_NONE;
}

// Impronta della chiave per i log (primi 4 byte di HMAC-SHA256(sessionKey, "fingerprint")):
// permette di confrontare che i due lati abbiano la stessa chiave senza stamparla.
inline String wirelessKeyFingerprint(const uint8_t sessionKey[WIRELESS_SESSION_KEY_LEN])
{
  static const char label[] = "fingerprint";
  uint8_t out[32];
  const mbedtls_md_info_t *md = mbedtls_md_info_from_type(MBEDTLS_MD_SHA256);
  if (md == nullptr || mbedtls_md_hmac(md, sessionKey, WIRELESS_SESSION_KEY_LEN,
                                       (const unsigned char *)label, sizeof(label) - 1, out) != 0)
    return "????????";
  return wirelessToHex(out, 4);
}

#endif // WIRELESS_PROTOCOL_H
