<template>
  <div ref="modalRef" id="AlertModalContainer" class="modal fade" role="dialog" tabindex="-1">
    <div class="modal-dialog modal-dialog-centered" role="document">
      <div class="modal-content">
        <div class="modal-header">
          <h4 class="modal-title">{{ title }}</h4>
          <button class="btn-close" type="button" aria-label="Close" data-bs-dismiss="modal"></button>
        </div>
        <div class="modal-body">
          <p style="margin-bottom: 0px;">{{ message }}</p>
        </div>
        <div class="modal-footer">
          <button class="btn btn-light" type="button" data-bs-dismiss="modal">Close</button>
        </div>
      </div>
    </div>
  </div>
</template>

<script setup>
import { ref, onMounted, onBeforeUnmount } from 'vue'
import { Modal } from 'bootstrap'

// Nell'originale il costruttore chiamava subito render("Prova", "ProvaMessage")
// come valori placeholder di debug; qui usiamo stringhe vuote di default.
const props = defineProps({
  initialTitle: { type: String, default: '' },
  initialMessage: { type: String, default: '' },
})

const title = ref(props.initialTitle)
const message = ref(props.initialMessage)
const modalRef = ref(null)
let modalInstance = null

onMounted(() => {
  modalInstance = new Modal(modalRef.value)
})
onBeforeUnmount(() => {
  modalInstance?.dispose()
})

function alertMsg(newTitle, newMessage) {
  title.value = newTitle
  message.value = newMessage
  modalInstance?.show()
}

function show() {
  modalInstance?.show()
}

function hide() {
  modalInstance?.hide()
}

defineExpose({ alert: alertMsg, show, hide })
</script>
