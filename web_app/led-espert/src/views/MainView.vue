<template>
  <div>
    <div id="ContainAction-4" class="d-lg-flex justify-content-lg-center ContainAction">
      <div class="container containerLabel" style="height: auto; max-height: 25px;">
        <label class="form-label form-label-title" style="padding-top: 0px; padding-right: 0px; padding-bottom: 0px; padding-left: 0px;">Effetto</label>
        <svg
          class="bi bi-arrow-clockwise fs-4 border rounded-circle btn btn-dark"
          xmlns="http://www.w3.org/2000/svg" width="1em" height="1em" fill="currentColor" viewBox="0 0 16 16"
          data-bs-toggle="tooltip" data-bss-tooltip style="padding-top: 0px; padding-right: 0px; padding-bottom: 0px; padding-left: 0px;"
          title="Aggiorna effetti"
          @click="$emit('update-effects')"
        >
          <path fill-rule="evenodd" d="M8 3a5 5 0 1 0 4.546 2.914.5.5 0 0 1 .908-.417A6 6 0 1 1 8 2z"></path>
          <path d="M8 4.466V.534a.25.25 0 0 1 .41-.192l2.36 1.966c.12.1.12.284 0 .384L8.41 4.658A.25.25 0 0 1 8 4.466"></path>
        </svg>
      </div>
      <div class="d-lg-flex flex-column justify-content-center align-items-center align-items-sm-center align-items-md-center align-items-lg-center align-items-xl-center align-items-xxl-center containerOfComponent">
        <select
          v-model="effect"
          class="border rounded-pill d-flex effectInput form-control"
          style="max-width: 250px; font-size: 11px; text-align: center;"
          name="effect select" required
          @change="onEffectChange"
        >
          <option v-for="e in listEffects" :key="e.name" :value="e.name">{{ formatEffectName(e.name) }}</option>
        </select>
      </div>
    </div>

    <div id="ContainAction-3" class="ContainAction">
      <div class="container containerLabel" style="height: auto; max-height: 25px;"><label class="form-label componentForm form-label-title">Tempo (ms)</label></div>
      <div class="d-flex flex-column containerOfComponent" style="overflow: hidden;">
        <div class="border rounded-pill d-flex d-md-flex d-xxl-flex justify-content-center align-items-center align-content-center justify-content-md-center justify-content-xxl-center componentForm rangeForm dark" data-bs-theme="dark" style="padding: 0px; padding-right: 12px; padding-left: 12px; padding-top: 10px; width: auto; --bs-body-bg: #2b3035; background: var(--bs-body-bg);">
          <input
            v-model.number="deltaT"
            class="form-range border-1 border-secondary focus-ring focus-ring-secondary d-flex align-content-center align-self-center justify-content-md-center componentForm rangeForm timingRangeInput"
            type="range" name="timingSlide" min="50" max="1000" step="1" title="Value" data-bs-theme="dark"
            style="max-width: 420px;" required
          />
          <div class="d-flex flex-row justify-content-center align-items-center">
            <input
              v-model.number="deltaT"
              class="form-control-sm d-md-flex justify-content-md-center timingInput"
              type="number" name="timing" min="50" max="1000" step="1"
              style="max-width: 70px; width: auto; margin-bottom: 10px; margin-right: 5px; margin-left: 5px;"
            />
            <label class="form-label" style="margin-right: 5px; margin-left: 2px;">ms</label>
          </div>
        </div>
      </div>
    </div>

    <div id="ContainAction-2" class="ContainAction">
      <div class="container containerLabel" style="height: auto; max-height: 25px;"><label class="form-label form-label-title">Colore</label></div>
      <div class="containerOfComponent">
        <GroupColorView
          ref="groupColorRef"
          :colors="initialModel.colors"
          :min="minColors"
          :max="maxColors"
          :only-read="false"
          :can-add-or-remove-color="true"
        />
      </div>
    </div>

    <div id="ContainAction-1" class="ContainAction">
      <div class=" container containerLabel container-info-strip-leds" style="height: auto; max-height: 25px;"><label class="form-label componentForm form-label-title">Striscia led soggetta all'effetto</label></div>
      <div class="panel-info-strip-leds d-flex flex-column justify-content-xl-center align-items-xl-center containerOfComponent" style="overflow: hidden;">
        <div class=" row justify-content-xl-center align-items-xl-center align text-center border rounded-pill d-flex d-md-flex d-xxl-flex flex-row justify-content-center align-items-center align-content-center align-self-center m-auto justify-content-lg-center justify-content-xxl-center componentForm rangeForm dark" data-bs-theme="dark" style="padding: 0px; padding-right: 12px; padding-left: 12px; padding-top: 10px; width: auto; --bs-body-bg: #2b3035; background: var(--bs-body-bg); min-width: 200px;">
          <div class="form-check text-center d-flex d-lg-flex flex-column justify-content-center align-items-center align-content-center align-self-center flex-wrap justify-content-lg-center align-items-lg-center" data-bs-theme="dark" style="min-width: auto; margin-right: 5px; margin-top: 2px; margin-left: 5px; padding: 0; width: auto; padding-top: 0; padding-right: 0; padding-bottom: 0; padding-left: 0; margin-bottom: 2px;">
            <input id="rgbCheck" v-model="rgbCheck" class="form-check-input" type="checkbox" style="margin-left: 0px; width: 30px; height: 30px;" name="RGB" />
            <label class="form-check-label text-center d-flex flex-fill justify-content-center align-items-center" for="rgbCheck" style="position: relative; margin-right: 0px; margin-left: 0px; text-align: center; max-width: initial; min-width: 75px;">RGB</label>
          </div>
          <div class="form-check text-center d-flex d-lg-flex flex-column justify-content-center align-items-center align-content-center justify-content-lg-center align-items-lg-center" data-bs-theme="dark" style="padding: 0; padding-left: 0; padding-right: 0; margin-right: 5px; padding-top: 0; margin-top: 2px; min-width: auto; margin-left: 5px; text-align: center; width: auto; padding-bottom: 0; margin-bottom: 2px;">
            <input id="ws2811Check" v-model="ws2811Check" class="form-check-input d-flex d-lg-flex justify-content-center align-items-center justify-content-lg-center align-items-lg-center" type="checkbox" style="margin-left: 0px; width: 30px; height: 30px;" name="WS2811" />
            <label class="form-check-label d-flex d-lg-flex flex-fill justify-content-center align-items-center align-self-center justify-content-lg-center align-items-lg-center" for="ws2811Check" style="margin-left: 0px; margin-right: 0px; position: relative; text-align: center; max-width: initial; min-width: 75px;">WS2811</label>
          </div>
          <div class="form-check text-center d-flex d-lg-flex flex-column justify-content-center align-items-center align-content-center justify-content-lg-center align-items-lg-center" data-bs-theme="dark" style="padding: 0; padding-left: 0; padding-right: 0; margin-right: 5px; padding-top: 0; margin-top: 2px; min-width: auto; margin-left: 5px; text-align: center; width: auto; padding-bottom: 0; margin-bottom: 2px;">
            <input id="ws2811MatrixCheck" v-model="ws2811MatrixCheck" class="form-check-input d-flex d-lg-flex justify-content-center align-items-center justify-content-lg-center align-items-lg-center" type="checkbox" style="margin-left: 0px; width: 30px; height: 30px;" name="WS2811" />
            <label class="form-check-label d-flex d-lg-flex flex-fill justify-content-center align-items-center align-self-center justify-content-lg-center align-items-lg-center" for="ws2811MatrixCheck" style="margin-left: 0px; margin-right: 0px; position: relative; text-align: center; max-width: initial; min-width: 75px;">Matrice</label>
          </div>
        </div>
      </div>
    </div>

    <div id="ContainAction-5" class="d-lg-flex justify-content-center align-items-center">
      <div class="dropdown d-inline btn-black btn-dark">
        <button class="btn btn-dark dropdown-toggle btn-black btn-primary dark" aria-expanded="false" data-bs-toggle="dropdown" type="button">Gestisci Effetto Iniziale</button>
        <div class="dropdown-menu">
          <button class="btn btn-dark fs-5" type="button" @click="$emit('save-initial-effect')">SALVA EFFETTO INIZIALE</button>
          <button class="btn btn-dark fs-5" type="button" @click="$emit('clear-initial-effect')">RIMUOVI EFFETTO INIZIALE</button>
        </div>
      </div>
    </div>

    <div class="d-lg-flex justify-content-lg-center align-items-lg-center containerExecutiveLed">
      <div id="ContainAction-6" class="d-flex d-lg-flex flex-column justify-content-center align-items-center">
        <button class="btn btn-dark fs-5 py-2 px-4 mb-3 buttonForm" type="button" @click="onSendStart">INVIA INIZIO EFFETTO</button>
        <button class="btn btn-dark fs-5 py-2 px-4 mb-3 buttonForm" type="button" @click="$emit('stop-effect')">INVIA FINE EFFETTO</button>
      </div>
    </div>
  </div>
</template>

<script setup>
// Nota: nell'originale MainView.js c'erano riferimenti "orfani" a
// this.aPConnection e a un campo '.fieldIp' (getFieldIp(), isAPconnection())
// che però non compaiono da nessuna parte nel render() di QUESTA view: quel
// markup (checkbox "Connessione AP" + campo IP) esiste solo, commentato,
// in fondo al file originale ed è concettualmente competenza di
// Esp32ConnectionView, non di MainView. Sembra codice morto/residuo di un
// refactor: non l'ho riportato qui per evitare di duplicare responsabilità.
import { ref, computed } from 'vue'
import TextUtils from '../utils/TextUtils'
import LedMainModel from '../models/LedMainModel'
import GroupColorView from './GroupColorView.vue'

const props = defineProps({
  listEffects: { type: Array, default: () => [] },
  initialModel: { type: Object, default: () => new LedMainModel() },
})
const emit = defineEmits(['update-effects', 'save-initial-effect', 'clear-initial-effect', 'send-effect', 'stop-effect'])

const groupColorRef = ref(null)

const effect = ref(props.initialModel.effect)
const deltaT = ref(props.initialModel.deltaT)
const rgbCheck = ref(props.initialModel.rgbCheck ?? true)
const ws2811Check = ref(props.initialModel.ws2811Check ?? true)
const ws2811MatrixCheck = ref(props.initialModel.ws2811MAtrixCheck ?? true)

const minColors = ref(1)
const maxColors = ref(1)

function formatEffectName(name) {
  return TextUtils.convertUnderscoreIntoSpace(name)
}

function onEffectChange() {
  const selected = props.listEffects.find((e) => e.name === effect.value)
  if (selected) {
    minColors.value = selected.minColorsNumber
    maxColors.value = selected.maxColorsNumber
    groupColorRef.value?.hideAllColor()
    groupColorRef.value?.setBound(selected.minColorsNumber, selected.maxColorsNumber)
    groupColorRef.value?.showColors(selected.minColorsNumber)
  }
}

function getModel() {
  return new LedMainModel(
    undefined, // aPConnection: non gestito da questa view, vedi nota sopra
    undefined, // textFieldActualHost: idem
    effect.value,
    deltaT.value,
    groupColorRef.value?.getColors() ?? [],
    rgbCheck.value,
    ws2811Check.value,
    ws2811MatrixCheck.value,
  )
}

function onSendStart() {
  emit('send-effect', getModel())
}

defineExpose({ getModel })
</script>
