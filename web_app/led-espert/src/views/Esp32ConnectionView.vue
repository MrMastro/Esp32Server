<template>
  <div>
    <div class="container containerLabel" style="height: auto; max-height: 25px;">
      <label class="form-label form-label-title"
        style="padding-top: 0px; padding-right: 0px; padding-bottom: 0px; padding-left: 0px;">Connessioni Esp32</label>

      <svg class="bi bi-search fs-4 border rounded-circle btn btn-dark" xmlns="http://www.w3.org/2000/svg" width="1em"
        height="1em" fill="currentColor" viewBox="0 0 16 16" data-bs-toggle="tooltip" data-bss-tooltip
        title="Cerca dispositivi" style="padding-top: 0px; padding-right: 0px; padding-bottom: 0px; padding-left: 0px;"
        @click="$emit('search')" @pointerdown="onSearchPointerDown" @pointerup="onSearchPointerUp"
        @pointerleave="onSearchPointerLeave">
        <path
          d="M11.742 10.344a6.5 6.5 0 1 0-1.397 1.398h-.001c.03.04.062.078.098.115l3.85 3.85a1 1 0 0 0 1.415-1.414l-3.85-3.85a1.007 1.007 0 0 0-.115-.1zM12 6.5a5.5 5.5 0 1 1-11 0 5.5 5.5 0 0 1 11 0">
        </path>
      </svg>

      <svg class="bi bi-arrow-clockwise fs-4 border rounded-circle btn btn-dark" xmlns="http://www.w3.org/2000/svg"
        width="1em" height="1em" fill="currentColor" viewBox="0 0 16 16" data-bs-toggle="tooltip" data-bss-tooltip
        title="Aggiorna effetti" style="padding-top: 0px; padding-right: 0px; padding-bottom: 0px; padding-left: 0px;"
        @click="$emit('update-status')">
        <path fill-rule="evenodd" d="M8 3a5 5 0 1 0 4.546 2.914.5.5 0 0 1 .908-.417A6 6 0 1 1 8 2z"></path>
        <path d="M8 4.466V.534a.25.25 0 0 1 .41-.192l2.36 1.966c.12.1.12.284 0 .384L8.41 4.658A.25.25 0 0 1 8 4.466">
        </path>
      </svg>
    </div>

    <div class="panelConnections d-flex dark justify-content-center"
      style="width: auto; --bs-body-bg: #2b3035; background: var(--bs-body-bg); min-width: auto;">
      <div class="d-flex d-lg-flex flex-column align-items-center justify-content-center align-items-center"
        style="margin: 10px;">

        <template v-if="connections.length > 0">
          <div v-for="(esp32, index) in connections" :key="index"
            class="esp32PanelConnection d-flex justify-content-between align-items-center esp32PanelConnection">
            <input type="checkbox" class="elementEsp32" style="width: 25px; height: 25px;" :checked="esp32.active"
              :disabled="esp32.connectionState === OFFLINE"
              @change="$emit('toggle-connection', esp32, $event.target.checked)" />
            <span class="ms-2 elementEsp32" :class="'indicator-' + esp32.connectionState"
              style="width: 15px; height: 15px; border-radius: 50%; margin-right: 5px;"></span>
            <span class="text-start" style="width: 140px;">{{ esp32.infoConnection.deviceName }}</span>
            <svg class="bi bi-tools border rounded-circle" xmlns="http://www.w3.org/2000/svg" width="1em" height="1em"
              fill="currentColor" viewBox="0 0 16 16" style="width: 25px; height: 25px;"
              @click="$emit('info-click', esp32)">
              <path
                d="M1 0 0 1l2.2 3.081a1 1 0 0 0 .815.419h.07a1 1 0 0 1 .708.293l2.675 2.675-2.617 2.654A3.003 3.003 0 0 0 0 13a3 3 0 1 0 5.878-.851l2.654-2.617.968.968-.305.914a1 1 0 0 0 .242 1.023l3.27 3.27a.997.997 0 0 0 1.414 0l1.586-1.586a.997.997 0 0 0 0-1.414l-3.27-3.27a1 1 0 0 0-1.023-.242L10.5 9.5l-.96-.96 2.68-2.643A3.005 3.005 0 0 0 16 3c0-.269-.035-.53-.102-.777l-2.14 2.141L12 4l-.364-1.757L13.777.102a3 3 0 0 0-3.675 3.68L7.462 6.46 4.793 3.793a1 1 0 0 1-.293-.707v-.071a1 1 0 0 0-.419-.814zm9.646 10.646a.5.5 0 0 1 .708 0l2.914 2.915a.5.5 0 0 1-.707.707l-2.915-2.914a.5.5 0 0 1 0-.708M3 11l.471.242.529.026.287.445.445.287.026.529L5 13l-.242.471-.026.529-.445.287-.287.445-.529.026L3 15l-.471-.242L2 14.732l-.287-.445L1.268 14l-.026-.529L1 13l.242-.471.026-.529.445-.287.287-.445.529-.026z">
              </path>
            </svg>
          </div>
        </template>
        <template v-else>
          <label class="form-label text-white">Nessun Dispositivo disponibile.<br />Cerca dispositivi cliccando sulla
            lente di ingrandimento</label>
          <label v-if="debugMessage" class="labelWrong">{{ debugMessage }}</label>
        </template>

        <div class="panelAdvancedSearch d-flex flex-column justify-content-center align-items-center"
          :class="{ hidden: !advancedSearchMode }">
          <div class="border rounded d-flex justify-content-between align-items-center esp32PanelConnection"
            style="margin: 10px 10px 0px 0px;">
            <input v-model="ipInput" class="fieldIp" type="text" minlength="7" maxlength="15" size="15"
              pattern="^((\d{1,2}|1\d\d|2[0-4]\d|25[0-5])\.){3}(\d{1,2}|1\d\d|2[0-4]\d|25[0-5])$" @input="onIpInput" />
            <svg v-show="ipValid" class="bi bi-search btn btn_svg_icon searchAdvanceEsp32" xmlns="http://www.w3.org/2000/svg"
              fill="currentColor" viewBox="0 0 16 16" style="width: 25px; height: 25px;"
              @click="changeIp(ipInput)"><!-- "$emit('advanced-search', ipInput)" -->
              <path
                d="M11.742 10.344a6.5 6.5 0 1 0-1.397 1.398h-.001c.03.04.062.078.098.115l3.85 3.85a1 1 0 0 0 1.415-1.414l-3.85-3.85a1.007 1.007 0 0 0-.115-.1zM12 6.5a5.5 5.5 0 1 1-11 0 5.5 5.5 0 0 1 11 0">
              </path>
            </svg>
            <svg v-show="!ipValid" class="bi bi-search btn disabled btn_svg_icon" viewBox="0 0 16 16"
              fill="currentColor" style="width: 25px; height: 25px;">
              <path
                d="M11.742 10.344a6.5 6.5 0 1 0-1.397 1.398h-.001c.03.04.062.078.098.115l3.85 3.85a1 1 0 0 0 1.415-1.414l-3.85-3.85a1.007 1.007 0 0 0-.115-.1zM12 6.5a5.5 5.5 0 1 1-11 0 5.5 5.5 0 0 1 11 0" />
              <line x1="2" y1="2" x2="11" y2="11" stroke="white" stroke-width="1" />
              <line x1="2" y1="11" x2="11" y2="2" stroke="white" stroke-width="1" />
            </svg>
          </div>
          <small class="label-ip" :class="ipLabelClass">{{ ipLabel }}</small>
        </div>

      </div>
    </div>
  </div>
</template>

<script setup>
import { ref } from 'vue'
import { ConnectionInfo } from '../models/Esp32Model.js'

// connections: array di { infoConnection, connectionState, active, espConnection }
// sostituisce l'array interno this.arrayConnections dell'originale, che
// veniva ricostruito ad ogni render() interrogando il DOM: qui il DOM
// diventa una semplice proiezione dei dati passati via prop.
const props = defineProps({
  connections: { type: Array, default: () => [] },
  debugMessage: { type: String, default: '' },
})
defineEmits(['toggle-connection', 'info-click', 'update-status', 'search', 'advanced-search'])

const OFFLINE = ConnectionInfo.OFFLINE

const advancedSearchMode = ref(false)
const ipInput = ref('')
const ipValid = ref(false)
const ipLabel = ref('Inserisci un indirizzo ip della rete wifi')
const ipLabelClass = ref('labelIpNeutral')

let pressTimer = null
let isLongPress = false

// Stessa logica press-and-hold di 1s dell'originale (bindLongButtonSearchEsp32)
// per aprire il pannello di ricerca avanzata; un click rapido emette 'search'.
function onSearchPointerDown() {
  isLongPress = false
  pressTimer = setTimeout(() => {
    isLongPress = true
    advancedSearchMode.value = true
  }, 1000)
}
function onSearchPointerUp() {
  clearTimeout(pressTimer)
  if (!isLongPress) {
    advancedSearchMode.value = false
  }
}
function onSearchPointerLeave() {
  clearTimeout(pressTimer)
}

function onIpInput() {
  const test = /^((\d{1,2}|1\d\d|2[0-4]\d|25[0-5])\.){3}(\d{1,2}|1\d\d|2[0-4]\d|25[0-5])$/.test(ipInput.value)
  if (!ipInput.value) {
    ipLabel.value = 'Inserisci un indirizzo ip della rete wifi'
    ipLabelClass.value = 'labelIpNeutral'
    ipValid.value = false
  } else if (test) {
    ipLabel.value = 'Clicca sulla lente per iniziare la ricerca'
    ipLabelClass.value = 'labelIpRight'
    ipValid.value = true
  } else {
    ipLabel.value = 'Inserisci un indirizzo ip della rete wifi valido'
    ipLabelClass.value = 'labelIpWrong'
    ipValid.value = false
  }
}

function changeIp(ip) {
  console.log("changeIp:"+ip);
  let test = /^((\d{1,2}|1\d\d|2[0-4]\d|25[0-5])\.){3}(\d{1,2}|1\d\d|2[0-4]\d|25[0-5])$/.test(ip);
  if (!ip && ip != ipInput.value) {
    ipLabel.value = "Inserisci un indirizzo ip della rete wifi";
    ipInput = null;
  }
  else if (test) {
    console.log(ipInput.value);
    // this.ipForAdvancedSearch = this.getFieldIp();
    // this.setLabelIp("Clicca sulla lente per iniziare la ricerca");
    // this.buttonAdvancedSearchEsp32Disabled.classList.add("hidden");
    // this.buttonAdvancedSearchEsp32.classList.remove("hidden");
    // this.labelIp.classList.add("labelIpRight");
    // this.labelIp.classList.remove("labelIpWrong");
    // this.labelIp.classList.remove("labelIpNeutral");
  } else {
    ipLabel.value = "Inserisci un indirizzo ip della rete wifi valido";
    // this.setLabelIp("Inserisci un indirizzo ip della rete wifi valido");
    // this.buttonAdvancedSearchEsp32Disabled.classList.remove("hidden");
    // this.buttonAdvancedSearchEsp32.classList.add("hidden");
    // this.labelIp.classList.remove("labelIpRight");
    // this.labelIp.classList.remove("labelIpNeutral");
    // this.labelIp.classList.add("labelIpWrong");
  }
}
</script>
