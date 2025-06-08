<template>
  <div class="not-found">
    <h1>404 - Page Not Found</h1>
    <p>The page you are looking for does not exist.</p>
    <a href="javascript:" @click.prevent="router.push('/')">Go to Home</a>
  </div>
</template>

<script setup>
import { useRouter } from 'vue-router';
const router = useRouter();

</script>

<style scoped>
.not-found {
  text-align: center;
  padding: 2rem;
}
.not-found h1 {
  color: #ff4757;
  margin-bottom: 1rem;
}
.not-found p {
  margin-bottom: 1rem;
}
</style>