import { createRouter, createWebHashHistory } from 'vue-router'
const routes = [
  {
    path: '/',
    redirect: '/app/'
  },
  {
    path: '/app/',
    component: () => import('../components/Main.vue')
  },
  {
    path: '/:pathMatch(.*)*',
    component: () => import('../components/NotFound.vue')
  }
]

const router = createRouter({
  history: createWebHashHistory(),
  routes
})

export default router
