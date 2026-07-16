<template>
  <div ref="modalRef" class="modal fade" role="dialog" tabindex="-1" aria-labelledby="loginModalLabel" aria-hidden="true">
    <div class="modal-dialog" role="document">
      <div class="modal-content">
        <div class="modal-header">
          <h1 class="modal-title d-flex justify-content-center align-items-center">Info</h1>
          <button type="button" class="btn-close" data-bs-dismiss="modal" aria-label="Close"></button>
        </div>

        <div class="modal-body">
          <form
            ref="formRef"
            class="p-4 py-5 formLoginForUpdate"
            novalidate
            :class="{ 'was-validated': wasValidated }"
            @submit.prevent="onSubmit"
          >
            <div class="mb-3">
              <label class="form-label" for="deviceNameLogin">Nome Dispositivo</label>
              <input id="deviceNameLogin" :value="deviceName" class="form-control item" type="text" required minlength="1" readonly />
            </div>

            <div class="mb-3">
              <label class="form-label" for="ip">Indirizzo Ip</label>
              <input id="ip" :value="ip" class="form-control item" type="text" required minlength="1" readonly />
            </div>

            <div class="mb-3">
              <label class="form-label">Stato</label>
              <div class="groupStato d-flex align-items-center">
                <input :value="connectionState" class="form-control item" type="text" required minlength="1" readonly />
                <span class="ms-2 item" :class="'indicator-' + connectionState" style="width: 15px; height: 15px; border-radius: 50%;"></span>
              </div>
            </div>

            <div class="mb-3">
              <label class="form-label" for="passwordLogin">Password</label>
              <input id="passwordLogin" v-model="password" class="form-control passwordInput" type="password" required minlength="1" />
              <label class="labelWrong" :class="{ hidden: !showPasswordError }" id="passwordLoginLabel">
                Inserire una password valida per accedere alle impostazioni
              </label>
            </div>

            <button id="submitLoginForUpdate" class="btn btn-primary" type="submit">Impostazioni</button>
          </form>
        </div>

        <div class="modal-footer">
          <button type="button" class="btn btn-secondary" data-bs-dismiss="modal">Close</button>
        </div>
      </div>
    </div>
  </div>
</template>

<script setup>
import { ref, reactive, onMounted, onBeforeUnmount } from 'vue'
import { Modal } from 'bootstrap'

// Nota importante: l'originale usava jQuery ($(this.modal).modal('show'/'hide'))
// per pilotare il modale mentre WaitView/AlertMessageView usavano l'API nativa
// di Bootstrap (new bootstrap.Modal(...)). Qui uniformo tutto all'API nativa
// di Bootstrap, eliminando così la dipendenza da jQuery in questo componente.
const emit = defineEmits(['login'])

const modalRef = ref(null)
const formRef = ref(null)
let modalInstance = null

const deviceName = ref('')
const ip = ref('')
const connectionState = ref('')
const password = ref('')
const wasValidated = ref(false)
const showPasswordError = ref(false)

let esp32Selected = null
// Cache in-memory delle ultime password inserite per MAC address, come
// nell'originale (this.passwordsData). Si perde al refresh della pagina:
// se serve persistenza reale, va salvata altrove (es. store, backend).
const passwordsByMac = reactive({})

onMounted(() => {
  modalInstance = new Modal(modalRef.value)
})
onBeforeUnmount(() => {
  modalInstance?.dispose()
})

function renderForm(esp32Model) {
  esp32Selected = esp32Model || null
  wasValidated.value = false
  showPasswordError.value = false

  if (esp32Model) {
    const connection = esp32Model.espConnection
    const infoConnection = connection.infoConnection
    deviceName.value = infoConnection.deviceName
    connectionState.value = connection.connectionState
    ip.value = infoConnection.ip
    password.value = passwordsByMac[infoConnection.macAdress] || ''
  } else {
    deviceName.value = ''
    connectionState.value = ''
    ip.value = ''
    password.value = ''
  }
}

function onSubmit() {
  if (!formRef.value.checkValidity()) {
    showPasswordError.value = true
    wasValidated.value = true
    return
  }
  showPasswordError.value = false
  const macAddress = esp32Selected.espConnection.infoConnection.macAdress
  passwordsByMac[macAddress] = password.value
  emit('login', esp32Selected, password.value)
}

function show(esp32Model) {
  renderForm(esp32Model)
  modalInstance?.show()
}

function hide() {
  modalInstance?.hide()
}

defineExpose({ show, hide })
</script>
