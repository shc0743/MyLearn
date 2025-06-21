import { defineConfig } from 'vite'
import vue from '@vitejs/plugin-vue'

// https://vite.dev/config/
export default defineConfig({
  plugins: [vue({
    template: {
      compilerOptions: {
        isCustomElement: (tag) => ['resizable-widget', 'x-canvas-api-draw-im-command'].includes(tag)
      }
    }
  })],
  build: {
    target: 'es2022',
    sourcemap: true
  },
  base: '/webstatic/my-ai-canvas-painting/',
})
