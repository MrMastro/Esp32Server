<template>
  <div ref="modalRef" class="modal fade" role="dialog" tabindex="-1">
    <div class="modal-dialog" role="document">
      <div class="modal-content">
        <div class="modal-header d-flex flex-column justify-content-xl-center align-items-xl-center" style="text-align: center; display: flex; position: relative;">
          <button class="btn-close" type="button" aria-label="Close" data-bs-dismiss="modal"></button>
          <h1 class="modal-title d-flex d-xl-flex justify-content-xl-center align-items-xl-center">Impostazioni</h1>
        </div>

        <div class="modal-body d-xl-flex flex-column align-self-stretch justify-content-xl-center align-items-xl-center">

          <div class="card" style="margin-bottom: 10px; margin-top: 10px; min-width: initial; max-width: initial;">
            <h2>Impostazioni Dispositivo</h2>
            <div class="card-body">
              <div class="d-flex flex-column align-items-xl-center" style="margin-top: 10px; margin-bottom: 10px;">
                <h4>Impostazioni Principali</h4>

                <div class="d-xl-flex flex-row justify-content-xl-center align-items-xl-center ContainAction">
                  <div class="container d-xl-flex containerLabel" style="height: auto; max-height: 25px;">
                    <label class="form-label form-label-title" style="font-size: 12px; text-align: left; min-width: 90px;">Nome Dispositivo</label>
                  </div>
                  <div class="text-center d-xl-flex flex-column justify-content-xl-center align-items-xl-center containerOfComponent">
                    <div class="text-center d-xl-flex justify-content-xl-center align-items-xl-center">
                      <input v-model="form.deviceName" type="text" style="width: 175px;" />
                    </div>
                  </div>
                </div>

                <div class="d-xl-flex flex-row justify-content-xl-center align-items-xl-center ContainAction">
                  <div class="container d-xl-flex containerLabel" style="height: auto; max-height: 25px;">
                    <label class="form-label text-center form-label-title" style="font-size: 12px; text-align: left; min-width: 90px;">Password Dispositivo</label>
                  </div>
                  <div class="text-center d-xl-flex flex-column justify-content-xl-center align-items-xl-center containerOfComponent">
                    <div class="text-center d-xl-flex justify-content-xl-center align-items-xl-center">
                      <input v-model="form.devicePassword" type="password" style="width: 175px;" />
                    </div>
                  </div>
                </div>

                <div class="d-xl-flex flex-row justify-content-xl-center align-items-xl-center ContainAction">
                  <div class="container containerLabel" style="height: auto; max-height: 25px;">
                    <label class="form-label text-center form-label-title" style="font-size: 12px; text-align: left; min-width: 90px;">Tipologia di comunicazione</label>
                  </div>
                  <div class="text-center d-xl-flex flex-column justify-content-xl-center align-items-xl-center containerOfComponent">
                    <div class="text-center d-xl-flex justify-content-xl-center align-items-xl-center">
                      <select v-model="form.communicationMode" class="border rounded-pill form-select-sm d-flex justify-content-center" style="font-size: 11px; text-align: center; width: 175px;" required>
                        <option value="AP_MODE">Access Point</option>
                        <option value="WIFI_MODE">WiFi</option>
                        <option value="BLUETOOTH_MODE">Bluetooth</option>
                      </select>
                    </div>
                  </div>
                </div>

                <div class="d-xl-flex flex-row justify-content-xl-center align-items-xl-center ContainAction">
                  <div class="container containerLabel" style="height: auto; max-height: 25px;">
                    <label class="form-label text-center form-label-title" style="font-size: 12px; text-align: left; min-width: 90px;">Modalità Debug</label>
                  </div>
                  <div class="text-center d-xl-flex flex-column align-items-xl-center containerOfComponent" style="width: 175px; height: 40px;">
                    <div class="text-center d-xl-flex justify-content-xl-center align-items-xl-center">
                      <input v-model="form.debug" class="d-xl-flex justify-content-xl-center align-items-xl-start" type="checkbox" style="width: 30px; height: 30px;" />
                    </div>
                  </div>
                </div>
              </div>

              <!-- Nell'originale queste due sezioni (AP e WIFI) erano sempre
                   entrambe visibili, indipendentemente dal valore di
                   communicationMode selezionato sopra: nessuna logica di
                   show/hide era presente nel codice sorgente. Ho mantenuto lo
                   stesso comportamento 1:1 per fedeltà, ma è un candidato
                   naturale per un v-if="form.communicationMode === 'AP_MODE'"
                   se vuoi migliorarlo. -->
              <div class="d-flex flex-column" style="margin-top: 10px; margin-bottom: 10px;">
                <h4>Impostazioni Connessione AP</h4>
                <div class="d-xl-flex flex-row justify-content-xl-center align-items-xl-center ContainAction">
                  <div class="container containerLabel" style="height: auto; max-height: 25px;">
                    <label class="form-label text-center form-label-title" style="font-size: 12px; text-align: left; min-width: 90px;">Nome Access Point</label>
                  </div>
                  <div class="text-center d-xl-flex flex-column justify-content-xl-center align-items-xl-center containerOfComponent">
                    <div class="text-center d-xl-flex justify-content-xl-center align-items-xl-center">
                      <input v-model="form.ssidAP" type="text" style="width: 175px;" />
                    </div>
                  </div>
                </div>
                <div class="d-xl-flex flex-row justify-content-xl-center align-items-xl-center ContainAction">
                  <div class="container containerLabel" style="height: auto; max-height: 25px;">
                    <label class="form-label text-center form-label-title" style="font-size: 12px; text-align: left; min-width: 90px;">Password Access Point</label>
                  </div>
                  <div class="text-center d-xl-flex flex-column justify-content-xl-center align-items-xl-center containerOfComponent">
                    <div class="text-center d-xl-flex justify-content-xl-center align-items-xl-center">
                      <input v-model="form.passwordAP" type="password" style="width: 175px;" />
                    </div>
                  </div>
                </div>
              </div>

              <div class="d-flex flex-column" style="margin-top: 10px; margin-bottom: 10px;">
                <h4>Impostazioni Connessione WIFI</h4>
                <div class="d-xl-flex flex-row justify-content-xl-center align-items-xl-center ContainAction">
                  <div class="container containerLabel" style="height: auto; max-height: 25px;">
                    <label class="form-label text-center form-label-title" style="font-size: 12px; text-align: left; width: 90px; min-width: 90px;">Nome rete</label>
                  </div>
                  <div class="text-center d-xl-flex flex-column justify-content-xl-center align-items-xl-center containerOfComponent">
                    <div class="text-center d-xl-flex justify-content-xl-center align-items-xl-center">
                      <input v-model="form.ssidWIFI" type="text" style="width: 175px;" />
                    </div>
                  </div>
                </div>
                <div class="d-xl-flex flex-row justify-content-xl-center align-items-xl-center ContainAction">
                  <div class="container containerLabel" style="height: auto; max-height: 25px;">
                    <label class="form-label text-center form-label-title" style="font-size: 12px; text-align: left; min-width: 90px;">Password rete</label>
                  </div>
                  <div class="text-center d-xl-flex flex-column justify-content-xl-center align-items-xl-center containerOfComponent">
                    <div class="text-center d-xl-flex justify-content-xl-center align-items-xl-center">
                      <input v-model="form.passwordWIFI" type="password" style="width: 175px;" />
                    </div>
                  </div>
                </div>
              </div>
            </div>
          </div>

          <div class="card" style="margin-top: 10px; margin-bottom: 10px; min-width: initial; max-width: initial;">
            <h2>Impostazioni Luci Led</h2>
            <div class="card-body" style="margin-top: 5px; margin-bottom: 5px;">
              <div style="margin-top: 10px; margin-bottom: 10px;">
                <h4>Impostazioni Strisce led</h4>

                <div class="d-flex d-xl-flex flex-row justify-content-center align-items-center justify-content-xl-center align-items-xl-center ContainAction" style="height: 95px;">
                  <div class="container containerLabel" style="height: auto; max-height: 25px;">
                    <label class="form-label text-center form-label-title" style="font-size: 12px; text-align: left; min-width: initial;">Striscia RGB</label>
                  </div>
                  <div class="text-center d-xl-flex flex-row justify-content-xl-center align-items-xl-center containerOfComponent">
                    <div class="d-flex d-xl-flex flex-column flex-shrink-1 justify-content-center align-items-center m-auto justify-content-xl-end align-items-xl-center" style="width: 88px; height: 65px; margin: initial; margin-top: initial; margin-left: initial; margin-right: 0px;">
                      <input v-model="form.ledSettings.enableStripRgb" class="d-xl-flex justify-content-xl-center" type="checkbox" style="width: 50px; height: 30px;" />
                      <label class="form-label">Abilitato</label>
                    </div>
                    <div class="d-flex d-xl-flex flex-column flex-shrink-1 justify-content-center align-items-center m-auto justify-content-xl-end align-items-xl-center" style="width: 88px; height: 65px;">
                      <input v-model="form.ledSettings.pinLedCinRgb" class="d-xl-flex justify-content-xl-center" type="number" style="max-width: 55px; margin-right: 5px; margin-left: 5px;" />
                      <label class="form-label" style="min-width: 72px;">Pin Cin</label>
                    </div>
                    <div class="d-flex d-xl-flex flex-column flex-shrink-1 justify-content-center align-items-center m-auto justify-content-xl-end align-items-xl-center" style="width: 88px; height: 65px; margin: initial; margin-top: initial; margin-left: initial; margin-right: 0px;">
                      <input v-model="form.ledSettings.pinLedDinRgb" class="d-xl-flex justify-content-xl-center" type="number" style="max-width: 53px;" />
                      <label class="form-label">Pin Din</label>
                    </div>
                  </div>
                </div>

                <div class="d-flex d-xl-flex flex-row align-items-center justify-content-lg-center align-items-lg-center justify-content-xl-center align-items-xl-center ContainAction" style="height: 95px;">
                  <div class="container containerLabel" style="height: auto; max-height: 25px;">
                    <label class="form-label text-center form-label-title" style="font-size: 12px; text-align: left; min-width: initial;">Striscia WS2811</label>
                  </div>
                  <div class="text-center d-flex d-lg-flex d-xl-flex flex-row justify-content-lg-center align-items-lg-center justify-content-xl-center align-items-xl-center containerOfComponent">
                    <div class="d-flex d-xl-flex flex-column flex-shrink-1 justify-content-center align-items-center m-auto justify-content-lg-center align-items-lg-center justify-content-xl-end align-items-xl-center" style="width: 88px; height: 65px; margin: initial; margin-top: initial; margin-left: initial; margin-right: 0px;">
                      <input v-model="form.ledSettings.enableStripWs2811" class="d-xl-flex justify-content-xl-center" type="checkbox" style="width: 50px; height: 30px;" />
                      <label class="form-label">Abilitato</label>
                    </div>
                    <div class="d-flex d-xl-flex flex-column flex-shrink-1 justify-content-center align-items-center m-auto justify-content-lg-center align-items-lg-center justify-content-xl-end align-items-xl-center" style="width: 88px; height: 65px;">
                      <input v-model="form.ledSettings.numLedWs2811" class="d-xl-flex justify-content-xl-center" type="number" style="max-width: 55px;" />
                      <label class="form-label" style="min-width: 72px;">Qt Pixel</label>
                    </div>
                    <div class="d-flex d-xl-flex flex-column flex-shrink-1 justify-content-center align-items-center m-auto justify-content-lg-center align-items-lg-center justify-content-xl-end align-items-xl-center" style="width: 88px; height: 65px; margin: initial; margin-top: initial; margin-left: initial; margin-right: 0px;">
                      <input v-model="form.ledSettings.pinLedWs2811" class="d-xl-flex justify-content-xl-center" type="number" style="max-width: 53px;" />
                      <label class="form-label">Pin Din</label>
                    </div>
                  </div>
                </div>

                <div class="d-flex d-xl-flex flex-row align-items-center justify-content-lg-center align-items-lg-center justify-content-xl-center align-items-xl-center ContainAction" style="height: 95px;">
                  <div class="container containerLabel" style="height: auto; max-height: 25px;">
                    <label class="form-label text-center form-label-title" style="font-size: 12px; text-align: left; min-width: initial;">Matrice WS2811</label>
                  </div>
                  <div class="text-center d-flex d-lg-flex d-xl-flex flex-row justify-content-lg-center align-items-lg-center justify-content-xl-center align-items-xl-center containerOfComponent">
                    <div class="d-flex d-xl-flex flex-column flex-shrink-1 justify-content-center align-items-center m-auto justify-content-lg-center align-items-lg-center justify-content-xl-end align-items-xl-center" style="width: 88px; height: 65px; margin: initial; margin-top: initial; margin-left: initial; margin-right: 0px;">
                      <input v-model="form.ledSettings.enableStripWs2811Matrix" class="d-xl-flex justify-content-xl-center" type="checkbox" style="width: 50px; height: 30px;" />
                      <label class="form-label">Abilitato</label>
                    </div>
                    <div class="d-flex d-xl-flex flex-column flex-shrink-1 justify-content-center align-items-center m-auto justify-content-lg-center align-items-lg-center justify-content-xl-end align-items-xl-center" style="width: 88px; height: 65px;">
                      <input v-model="form.ledSettings.numLedWs2811Matrix" class="d-xl-flex justify-content-xl-center" type="number" style="max-width: 55px;" />
                      <label class="form-label" style="min-width: 72px;">Qt Pixel</label>
                    </div>
                    <div class="d-flex d-xl-flex flex-column flex-shrink-1 justify-content-center align-items-center m-auto justify-content-lg-center align-items-lg-center justify-content-xl-end align-items-xl-center" style="width: 88px; height: 65px; margin: initial; margin-top: initial; margin-left: initial; margin-right: 0px;">
                      <input v-model="form.ledSettings.pinLedWs2811Matrix" class="d-xl-flex justify-content-xl-center" type="number" style="max-width: 53px;" />
                      <label class="form-label">Pin Din</label>
                    </div>
                  </div>
                </div>
              </div>

              <div class="border rounded-0 d-flex flex-column visible" style="margin-top: 10px; margin-bottom: 10px;">
                <h4>Informazioni effetto iniziale</h4>
                <h6 class="text-center">(Effetto iniziale modificabile altrove)</h6>

                <div class="d-lg-flex d-xl-flex flex-row align-items-lg-center justify-content-xl-center align-items-xl-center ContainAction">
                  <div class="container containerLabel" style="height: auto; max-height: 25px;">
                    <label class="form-label text-center form-label-title" style="font-size: 12px; text-align: left; min-width: 90px;">Effetto iniziale</label>
                  </div>
                  <div class="text-center d-xl-flex flex-column justify-content-xl-center align-items-xl-center containerOfComponent">
                    <div class="text-center d-xl-flex justify-content-xl-center align-items-xl-center">
                      <input :value="form.initialEffect" class="d-lg-flex justify-content-lg-center align-items-lg-center" type="text" style="max-width: 150px; font-size: 11px; text-align: center; width: 150px;" readonly />
                    </div>
                  </div>
                </div>

                <div class="d-lg-flex d-xl-flex flex-row align-items-lg-center justify-content-xl-center align-items-xl-center ContainAction">
                  <div class="container containerLabel" style="height: auto; max-height: 25px;">
                    <label class="form-label text-center form-label-title" style="font-size: 12px; text-align: left; min-width: 90px;">Tempo di animazione</label>
                  </div>
                  <div class="text-center d-lg-flex d-xl-flex flex-row justify-content-lg-center align-items-lg-center justify-content-xl-center align-items-xl-center containerOfComponent">
                    <label class="form-label d-lg-flex d-xl-flex align-items-lg-center align-items-xl-end" style="margin-right: 5px; margin-left: 2px;">{{ form.initialDeltaT }}</label>
                    <label class="form-label d-lg-flex d-xl-flex align-items-lg-center align-items-xl-end" style="margin-right: 5px; margin-left: 2px;">ms</label>
                  </div>
                </div>

                <div class="d-lg-flex d-xl-flex flex-column justify-content-lg-center align-items-lg-center justify-content-xl-center align-items-xl-center ContainAction">
                  <div class="container containerLabel" style="height: auto; max-height: 25px;">
                    <label class="form-label text-center form-label-title" style="font-size: 12px; text-align: left; min-width: 90px;">
                      {{ form.initialColors && form.initialColors.length > 1 ? 'Colori iniziali' : 'Colore iniziale' }}
                    </label>
                  </div>
                  <div class="text-center d-lg-flex d-xl-flex flex-row justify-content-lg-center align-items-lg-center justify-content-xl-center align-items-xl-center containerOfComponent">
                    <GroupColorView
                      ref="groupColorRef"
                      :colors="form.initialColors"
                      :min="0"
                      :max="5"
                      :only-read="true"
                      :can-add-or-remove-color="false"
                    />
                  </div>
                </div>
              </div>
            </div>
          </div>

        </div>

        <div class="modal-footer">
          <button class="btn btn-light" type="button" data-bs-dismiss="modal">Annulla</button>
          <button class="btn btn-primary" type="button" @click="onSave">Salva</button>
        </div>
      </div>
    </div>
  </div>
</template>

<script setup>
import { ref, reactive, onMounted, onBeforeUnmount } from 'vue'
import { Modal } from 'bootstrap'
import GroupColorView from './GroupColorView.vue'
import SettingsModel from '../models/SettingModel.js'

const emit = defineEmits(['save'])

const modalRef = ref(null)
const groupColorRef = ref(null)
let modalInstance = null
let esp32Selected = null

const form = reactive({
  deviceName: '',
  devicePassword: '',
  communicationMode: 'AP_MODE',
  debug: false,
  ssidAP: '',
  passwordAP: '',
  ssidWIFI: '',
  passwordWIFI: '',
  initialEffect: '',
  initialDeltaT: 0,
  initialColors: [],
  ledSettings: {
    enableStripRgb: false,
    pinLedCinRgb: 0,
    pinLedDinRgb: 0,
    enableStripWs2811: false,
    enableStripWs2811Matrix: false,
    numLedWs2811: 0,
    numLedWs2811Matrix: 0,
    pinLedWs2811: 0,
    pinLedWs2811Matrix: 0,
  },
})

onMounted(() => {
  modalInstance = new Modal(modalRef.value)
})
onBeforeUnmount(() => {
  modalInstance?.dispose()
})

function applySettings(settings) {
  if (!settings) return
  form.deviceName = settings.deviceName
  form.devicePassword = settings.devicePassword
  form.communicationMode = settings.communicationMode
  form.debug = settings.debug
  form.ssidAP = settings.ssidAP
  form.passwordAP = settings.passwordAP
  form.ssidWIFI = settings.ssidWIFI
  form.passwordWIFI = settings.passwordWIFI
  form.initialEffect = settings.initialEffect
  form.initialDeltaT = settings.initialDeltaT
  form.initialColors = settings.initialColors
  Object.assign(form.ledSettings, settings.ledSettings)
}

function getSettings() {
  const colors = groupColorRef.value?.getColors() ?? []
  return new SettingsModel(
    form.deviceName,
    form.devicePassword,
    form.communicationMode,
    form.debug,
    form.initialEffect,
    form.initialDeltaT,
    colors,
    form.ssidAP,
    form.passwordAP,
    form.ssidWIFI,
    form.passwordWIFI,
    form.ledSettings.enableStripRgb,
    form.ledSettings.pinLedDinRgb,
    form.ledSettings.pinLedCinRgb,
    form.ledSettings.enableStripWs2811,
    form.ledSettings.enableStripWs2811Matrix,
    form.ledSettings.numLedWs2811,
    form.ledSettings.numLedWs2811Matrix,
    form.ledSettings.pinLedWs2811,
    form.ledSettings.pinLedWs2811Matrix,
  )
}

function onSave() {
  emit('save', getSettings())
}

function show(esp32Model, settings) {
  esp32Selected = esp32Model || esp32Selected
  applySettings(settings)
  modalInstance?.show()
}

function hide() {
  esp32Selected = null
  modalInstance?.hide()
}

defineExpose({ show, hide, getSettings })
</script>
