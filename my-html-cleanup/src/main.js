import { createApp } from 'vue'
import './style.css'
import App from './App.vue'
import router from './router'
const e = (await import('element-plus')).default
import 'element-plus/dist/index.css'
import 'element-plus/theme-chalk/dark/css-vars.css'

const app = createApp(App)
app.use(router)
app.use(e)
app.mount('vue-app')
for (const i of document.querySelectorAll('noscript,app-loading')) i.remove();
