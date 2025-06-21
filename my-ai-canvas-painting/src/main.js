import { createApp } from 'vue'
import './style.css'
import App from './App.vue'
import 'resizable-widget'
import { toast } from 'vue3-toastify'
import 'vue3-toastify/dist/index.css'

const app = createApp(App)
app.mount('#app')
