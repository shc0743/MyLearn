<template>
    <div class="container">
        <main>
            <RouterView></RouterView>
        </main>
    </div>
</template>

<script setup>

</script>

<style scoped>
.container {
    display: flex;
    flex-direction: column;
    overflow: hidden;
    position: absolute;
    inset: 0;
    z-index: 1;
}
main {
    flex: 1;
    overflow: auto;
    display: flex;
    flex-direction: column;
}
</style>
