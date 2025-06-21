import { createApp } from 'vue'
import './style.css'
import App from './App.vue'
import 'resizable-widget'
import './custom-render-and-im.js'
import 'vue3-toastify/dist/index.css'

const app = createApp(App)
app.mount('#app')
