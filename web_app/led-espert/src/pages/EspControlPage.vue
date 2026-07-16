<template>
  <div class="bg-dark text-white">
    <div class="bodyContainers clean-form dark ">
      <!-- Era: <header id="HeaderViewContainer"> generato da HeaderView -->
      <HeaderView @open-settings="onOpenSettings" />
      <section class="py-xl-5 clean-block dark" style="padding-top: 20px; margin-top: 0; padding-bottom: 20px;">
        <div
          class="text-white bg-primary border rounded border-0 p-4 py-5"
          style="margin-right: 10px; margin-left: 10px; margin-top: 0; margin-bottom: 10px; background: rgb(49,94,197);"
        >
          <div class="d-lg-flex containerAction">
  
            <!-- Era: <div id="Esp32ConnectionViewContainer"> -->
            <Esp32ConnectionView
              class="ContainAction"
              :connections="connections"
              @toggle-connection="onToggleConnection"
              @info-click="onOpenLogin"
              @update-status="onUpdateStatus"
              @search="onSearch"
              @advanced-search="onAdvancedSearch"
            />
  
            <!-- Era: <div id="MainViewContainer"> -->
            <MainView
              ref="mainViewRef"
              class="ContainAction"
              :list-effects="listEffects"
              :initial-model="initialLedModel"
              @update-effects="onUpdateEffects"
              @save-initial-effect="onSaveInitialEffect"
              @clear-initial-effect="onClearInitialEffect"
              @send-effect="onSendEffect"
              @stop-effect="onStopEffect"
            />
  
          </div>
  
          <div class="character-container ">
            <div class="character"></div>
          </div>
        </div>
      </section>
  
      <!-- Le modali/overlay: nell'originale erano pilotate imperativamente
           (show()/hide()) dai vari controller. Qui manteniamo la stessa API
           imperativa tramite template ref + defineExpose, invece di forzare
           tutto a v-if/v-model, per restare più fedeli al comportamento
           originale (bootstrap.Modal gestisce le sue transizioni). -->
      <SettingsView ref="settingsViewRef" @save="onSaveSettings" />
      <LoginView ref="loginViewRef" @login="onLogin" />
      <WaitView ref="waitViewRef" />
      <AlertMessageView ref="alertViewRef" />
  
      <!-- Era: <footer id="FooterViewContainer"> -->
      <FooterView />
  
    </div>
  </div>
</template>

<script setup>
import { ref } from 'vue';

import HeaderView from '../views/HeaderView.vue'
import Esp32ConnectionView from '../views/Esp32ConnectionView.vue'
import MainView from '../views/MainView.vue'
import SettingsView from '../views/SettingsView.vue'
import LoginView from '../views/LoginView.vue'
import WaitView from '../views/WaitView.vue'
import AlertMessageView from '../views/AlertMessageView.vue'
import FooterView from '../views/FooterView.vue'

// Props
const props = defineProps({
  connections: Array,
  listEffects: Array,
  initialLedModel: Object
});

// Emits
const emit = defineEmits([
  'toggle-connection',
  'open-login',
  'update-status',
  'search',
  'advanced-search',
  'update-effects',
  'save-initial-effect',
  'clear-initial-effect',
  'send-effect',
  'stop-effect',
  'open-settings', // Aggiunto per open-settings
  'save', // Aggiunto per save
  'login' // Aggiunto per login
]);

// Event handlers
const onToggleConnection = (connection) => {
  emit('toggle-connection', connection);
};

const onOpenLogin = () => {
  emit('open-login');
};

const onUpdateStatus = (status) => {
  emit('update-status', status);
};

const onSearch = (query) => {
  emit('search', query);
};

const onAdvancedSearch = (filters) => {
  emit('advanced-search', filters);
};

const onUpdateEffects = (effects) => {
  emit('update-effects', effects);
};

const onSaveInitialEffect = () => {
  emit('save-initial-effect');
};

const onClearInitialEffect = () => {
  emit('clear-initial-effect');
};

const onSendEffect = (effect) => {
  emit('send-effect', effect);
};

const onStopEffect = () => {
  emit('stop-effect');
};

// Definisci le funzioni per open-settings, save e login
const onOpenSettings = () => {
  emit('open-settings');
};

const onSaveSettings = () => {
  emit('save');
};

const onLogin = () => {
  emit('login');
};

// Refs per componenti figli futuri
const settingsViewRef = ref(null);
const loginViewRef = ref(null);
const waitViewRef = ref(null);
const alertViewRef = ref(null);

defineExpose({
  settingsViewRef,
  loginViewRef,
  waitViewRef,
  alertViewRef
});
</script>

<style scoped>
</style>