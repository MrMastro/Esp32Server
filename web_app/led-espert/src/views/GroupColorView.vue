<template>
  <div class="d-lg-flex flex-column justify-content-lg-center align-items-lg-center">
    <div
      class="text-center d-flex d-lg-flex d-xl-flex flex-row justify-content-lg-center align-items-lg-center justify-content-xl-center align-items-xl-center containerColor"
      style="margin-top: 5px; margin-bottom: 5px;"
    >
      <input
        v-for="(hex, index) in visibleColors"
        :key="index"
        type="color"
        class="d-lg-flex m-auto justify-content-lg-center align-items-lg-center colorInput form-control bg-dark"
        style="width: 50px; height: 40px;"
        :value="hex"
        :disabled="onlyRead"
        @input="onColorInput(index, $event.target.value)"
      />
    </div>

    <div v-if="canAddOrRemoveColor" style="margin-top: 5px; margin-bottom: 5px;">
      <svg
        class="bi bi-dash-circle fs-4 border rounded-circle btn btn-dark"
        xmlns="http://www.w3.org/2000/svg" width="1em" height="1em" fill="currentColor" viewBox="0 0 16 16"
        data-bs-toggle="tooltip" data-bss-tooltip
        :style="minusStyle"
        style="padding: 0; margin-right: 5px; margin-left: 5px;"
        @click="removeColor"
      >
        <path d="M8 15A7 7 0 1 1 8 1a7 7 0 0 1 0 14m0 1A8 8 0 1 0 8 0a8 8 0 0 0 0 16"></path>
        <path d="M4 8a.5.5 0 0 1 .5-.5h7a.5.5 0 0 1 0 1h-7A.5.5 0 0 1 4 8"></path>
      </svg>

      <svg
        class="bi bi-plus-circle fs-4 border rounded-circle btn btn-dark"
        xmlns="http://www.w3.org/2000/svg" width="1em" height="1em" fill="currentColor" viewBox="0 0 16 16"
        data-bs-toggle="tooltip" data-bss-tooltip
        :style="plusStyle"
        style="padding: 0; margin-right: 5px; margin-left: 5px;"
        @click="addColor"
      >
        <path d="M8 15A7 7 0 1 1 8 1a7 7 0 0 1 0 14m0 1A8 8 0 1 0 8 0a8 8 0 0 0 0 16"></path>
        <path d="M8 4a.5.5 0 0 1 .5.5v3h3a.5.5 0 0 1 0 1h-3v3a.5.5 0 0 1-1 0v-3h-3a.5.5 0 0 1 0-1h3v-3A.5.5 0 0 1 8 4"></path>
      </svg>
    </div>
  </div>
</template>

<script setup>
// Conversione di GroupColorView.js: nell'originale il numero massimo di slot
// colore renderizzabili era fisso a 5 (createColorsIntoPanel(root, 5)).
// Qui manteniamo la stessa costante.
import { ref, computed, watch } from 'vue'
import ColorUtils from '../utils/ColorUtils.js'
import LedColorRequest from '../models/request/LedColorRequest.js'

const MAX_SLOTS = 5

const props = defineProps({
  colors: { type: Array, default: () => [] }, // [{ r, g, b }, ...]
  min: { type: Number, default: 1 },
  max: { type: Number, default: 1 },
  onlyRead: { type: Boolean, default: false },
  canAddOrRemoveColor: { type: Boolean, default: true },
})

const emit = defineEmits(['change'])

const slotColors = ref(Array.from({ length: MAX_SLOTS }, () => '#000000'))
const colorQtActive = ref(props.colors.length || props.min || 1)
const minColorShowed = ref(props.min)
const maxColorShowed = ref(props.max)

function loadColors(inputColors, useColorByInputColors) {
  inputColors.forEach((c, i) => {
    if (i < MAX_SLOTS) {
      const hex = ColorUtils.rgbToHex(c.r, c.g, c.b)
      if (useColorByInputColors !== false) slotColors.value[i] = hex
    }
  })
  colorQtActive.value = inputColors.length || 1
}

loadColors(props.colors, true)

watch(() => props.colors, (newColors) => {
  loadColors(newColors, true)
})

const visibleColors = computed(() => slotColors.value.slice(0, colorQtActive.value))

const minusStyle = computed(() => ({
  pointerEvents: colorQtActive.value === minColorShowed.value ? 'none' : 'auto',
  opacity: colorQtActive.value === minColorShowed.value ? '0.5' : '1',
  cursor: colorQtActive.value === minColorShowed.value ? 'not-allowed' : 'pointer',
}))
const plusStyle = computed(() => ({
  pointerEvents: colorQtActive.value === maxColorShowed.value ? 'none' : 'auto',
  opacity: colorQtActive.value === maxColorShowed.value ? '0.5' : '1',
  cursor: colorQtActive.value === maxColorShowed.value ? 'not-allowed' : 'pointer',
}))

function onColorInput(index, hex) {
  slotColors.value[index] = hex
  emit('change', getColors())
}

// waitAddColor nell'originale evitava doppi click entro 200ms; qui usiamo un
// semplice flag booleano invece del TimeUtils.wait asincrono del progetto originale.
let waitAddColor = false
function withDebounce(fn) {
  if (waitAddColor) return
  waitAddColor = true
  fn()
  setTimeout(() => { waitAddColor = false }, 200)
}

function addColor() {
  withDebounce(() => {
    if (colorQtActive.value < maxColorShowed.value) {
      colorQtActive.value++
      emit('change', getColors())
    }
  })
}

function removeColor() {
  withDebounce(() => {
    if (colorQtActive.value > minColorShowed.value) {
      colorQtActive.value--
      emit('change', getColors())
    }
  })
}

function setBound(min, max) {
  minColorShowed.value = min
  maxColorShowed.value = max
}

function hideAllColor() {
  colorQtActive.value = 0
}

function showColors(n) {
  colorQtActive.value = n
}

function getColors() {
  return visibleColors.value.map((hex) => {
    const rgb = ColorUtils.hexToRgb(hex)
    return new LedColorRequest(rgb.r, rgb.g, rgb.b)
  })
}

defineExpose({ setBound, hideAllColor, showColors, getColors })
</script>
