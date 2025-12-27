#include <Arduino.h>
#include <WIFI.h>
#include <FS.h>
#include <unity.h>
#include <models/settingModel/SettingsModel.h>

void test_settings_model_creation_from_default(void) {
    // Creazione dell'oggetto SettingsModel con valori di default
    SettingsModel settings = SettingsModel::getDefault();

    // Verifica dei valori di default
    TEST_ASSERT_EQUAL_STRING("ESP32_Device", settings.deviceName.c_str());
    TEST_ASSERT_EQUAL_STRING("esp32password", settings.devicePassword.c_str());
    TEST_ASSERT_EQUAL_STRING("AP_MODE", settings.communicationMode.c_str());
    TEST_ASSERT_EQUAL(false, settings.debug);
    TEST_ASSERT_EQUAL_STRING("NO_EFFECT", settings.initialEffect.c_str());
    TEST_ASSERT_EQUAL_STRING("ESP32_AP", settings.ssidAP.c_str());
    TEST_ASSERT_EQUAL_STRING("ap_password", settings.passwordAP.c_str());
    TEST_ASSERT_EQUAL_STRING("Home_Network", settings.ssidWIFI.c_str());
    TEST_ASSERT_EQUAL_STRING("wifi_password", settings.passwordWIFI.c_str());
    TEST_ASSERT_EQUAL(100, settings.initialDeltaT);
    // TEST_ASSERT_EQUAL(0, settings.initialColors.initialR);
    // TEST_ASSERT_EQUAL(0, settings.initialColors.initialG);
    // TEST_ASSERT_EQUAL(0, settings.initialColors.initialB);

    // Verifica dei valori di default per LedSettingsModel
    LedSettingsModel ledSettings = settings.ledSettings;
    TEST_ASSERT_EQUAL(false, ledSettings.enableStripWs2811);
    TEST_ASSERT_EQUAL(0, ledSettings.numLedWs2811);
    TEST_ASSERT_EQUAL(-1, ledSettings.pinLedWs2811);
    TEST_ASSERT_EQUAL(false, ledSettings.enableStripRgb);
    TEST_ASSERT_EQUAL(-1, ledSettings.pinLedDinRgb);

    Serial.println("Default object json:");
    Serial.println(settings.toJson());
}

void test_settings_model_creation_from_json(void) {
    // Stringa JSON di test
    String json = "{\n"
                  "    \"deviceName\": \"ESP32_Device\",\n"
                  "    \"devicePassword\": \"esp32password\",\n"
                  "    \"communicationmode\": \"AP_MODE\",\n"
                  "    \"debug\": false,\n"
                  "    \"initialEffect\": \"NO_EFFECT\",\n"
                  "    \"ssidAP\": \"ESP32_AP\",\n"
                  "    \"passwordAP\": \"ap_password\",\n"
                  "    \"ssidWIFI\": \"Home_Network\",\n"
                  "    \"passwordWIFI\": \"wifi_password\",\n"
                  "    \"initialDeltaT\": 100,\n"
                  "    \"initialR\": 0,\n"
                  "    \"initialG\": 0,\n"
                  "    \"initialB\": 0\n"
                  "}";

    // Creazione dell'oggetto SettingsModel
    SettingsModel settings;
    bool result = settings.fromJson(json);

    // Verifica del risultato della deserializzazione
    TEST_ASSERT_TRUE(result);
}

void setup() {
    delay(1000);
    // Avvia l'unit test framework
    UNITY_BEGIN();

    // Esegui il test
    RUN_TEST(test_settings_model_creation_from_default);

    // Fine dei test
    UNITY_END();
}

void loop() {
    // Niente da fare qui
}