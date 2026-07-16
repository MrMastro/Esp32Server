<template>
  <div ref="modalRef" class="modal fade" tabindex="-1" aria-labelledby="waitViewModalLabel" aria-hidden="true">
    <div class="modal-dialog modal-dialog-centered">
      <div class="modal-content">
        <div class="modal-body d-flex justify-content-center align-items-center">
          <div class="spinner-border text-primary" role="status">
            <span class="visually-hidden">Loading...</span>
          </div>
        </div>
      </div>
    </div>
  </div>
</template>

<script setup>
import { ref, onMounted, onBeforeUnmount } from 'vue'
import { Modal } from 'bootstrap'

const modalRef = ref(null)
let modalInstance = null

onMounted(() => {
  modalInstance = new Modal(modalRef.value)
})
onBeforeUnmount(() => {
  modalInstance?.dispose()
})

function show() {
  modalInstance?.show()
}

function hide() {
  // Stesso ritardo di 1 secondo prima di nascondere presente nell'originale
  // (il commento originale parlava di "5 secondi" ma il valore era 1000ms).
  setTimeout(() => {
    modalInstance?.hide()
  }, 1000)
}

defineExpose({ show, hide })
</script>
