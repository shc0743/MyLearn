<template>
    <div class="MyMain">
        <div role="heading" class="MyTitle">
            <ElTooltip trigger="hover" effect="dark" content="Reset preferences" placement="bottom">
                <ElButton @click="resetPrefs" @keydown.capture.stop @focus="$event.target.dispatchEvent(new MouseEvent('mouseenter'))" @blur="$event.target.dispatchEvent(new MouseEvent('mouseleave'))"><ElIcon><svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 1024 1024"><path fill="currentColor" d="M764.288 214.592 512 466.88 259.712 214.592a31.936 31.936 0 0 0-45.12 45.12L466.752 512 214.528 764.224a31.936 31.936 0 1 0 45.12 45.184L512 557.184l252.288 252.288a31.936 31.936 0 0 0 45.12-45.12L557.12 512.064l252.288-252.352a31.936 31.936 0 1 0-45.12-45.184z"></path></svg></ElIcon></ElButton>
            </ElTooltip>
            <span>HTML {{ mode_text }} Tool</span>
            <ElButton @click="toggleDark"><ElIcon><svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 1024 1024"><path fill="currentColor" d="M512 704a192 192 0 1 0 0-384 192 192 0 0 0 0 384m0 64a256 256 0 1 1 0-512 256 256 0 0 1 0 512m0-704a32 32 0 0 1 32 32v64a32 32 0 0 1-64 0V96a32 32 0 0 1 32-32m0 768a32 32 0 0 1 32 32v64a32 32 0 1 1-64 0v-64a32 32 0 0 1 32-32M195.2 195.2a32 32 0 0 1 45.248 0l45.248 45.248a32 32 0 1 1-45.248 45.248L195.2 240.448a32 32 0 0 1 0-45.248zm543.104 543.104a32 32 0 0 1 45.248 0l45.248 45.248a32 32 0 0 1-45.248 45.248l-45.248-45.248a32 32 0 0 1 0-45.248M64 512a32 32 0 0 1 32-32h64a32 32 0 0 1 0 64H96a32 32 0 0 1-32-32m768 0a32 32 0 0 1 32-32h64a32 32 0 1 1 0 64h-64a32 32 0 0 1-32-32M195.2 828.8a32 32 0 0 1 0-45.248l45.248-45.248a32 32 0 0 1 45.248 45.248L240.448 828.8a32 32 0 0 1-45.248 0zm543.104-543.104a32 32 0 0 1 0-45.248l45.248-45.248a32 32 0 0 1 45.248 45.248l-45.248 45.248a32 32 0 0 1-45.248 0"></path></svg></ElIcon></ElButton>
        </div>
        <ElInput v-model="userContent" :disabled="isRunning" type="textarea" class="MyInput" placeholder="Input HTML here..." />
        <div class="MySegment MyStat">
            <span style="color: var(--el-color-info)">Length:&nbsp;</span>
            <span v-text="userContent.length"></span>
        </div>
        <div class="MySegment MyConfig MyNoMargin">
            <div class="MyConfigRow MyHideScroll MyScrollControlParent">
                <span>Mode:&nbsp;</span>
                <ElRadioGroup v-model="mode">
                    <ElRadio value="html5">HTML API</ElRadio>
                    <ElRadio value="dompurify">DOMPurify</ElRadio>
                    <ElRadio value="text">Text Only</ElRadio>
                </ElRadioGroup>
            </div>
            <div class="MyConfigRow" v-if="mode === 'dompurify'">
                <span :style="{ color: dpConfigValid ? '' : 'red', 'margin-right': '0.5em' }">Configuration</span>
                <ElInput @click="dpConfigDlg.showModal()" @keydown.enter="dpConfigDlg.showModal()" :style="`--el-input-text-color: ${ dpConfigValid ? '' : 'red' }`" v-model="dpConfig" placeholder="Input DOMPurify configuration here..." clearable show-word-limit maxlength="999999" />
            </div>
            <div v-if="mode === 'html5' && (!userSeenH5Warning)">
                <span style="color: #cc0000; margin-right: 0.5em; word-break: break-all;">* Cleanup only. No purification.</span>
                <a href="javascript:" @click.prevent="userSeenH5Warning = true">I see</a>
            </div>
            <div class="MyConfigRow MyHideScroll MyScrollControlParent" v-if="mode === 'html5'" @wheel.prevent="$event.target.closest('.MyScrollControlParent')?.scrollBy({ left: $event.deltaY, top: 0 })">
                <span style="margin-right: 0.5em;">Removes (scroll to see more):</span>
                <ElCheckbox v-model="h5config.removes.style">&lt;style&gt;</ElCheckbox>
                <ElCheckbox v-model="h5config.removes.link">&lt;link&gt;&nbsp;<span style="color: var(--el-color-info-light-3)">(not &lt;a&gt;)</span></ElCheckbox>
                <ElCheckbox v-model="h5config.removes.svg">SVG</ElCheckbox>
                <ElCheckbox v-model="h5config.remove_comments">HTML Comments</ElCheckbox>
                <ElCheckbox v-model="h5config.removes.script">script</ElCheckbox>
                <ElCheckbox v-model="h5config.removes.iframe">iframe</ElCheckbox>
                <ElCheckbox v-model="h5config.removes.meta">meta</ElCheckbox>
                <ElCheckbox v-model="h5config.remove_style_attr">[style]&nbsp;<span style="color: var(--el-color-info-light-3)">attr</span></ElCheckbox>
                <ElCheckbox v-model="h5config.removes.a">&lt;a&gt;</ElCheckbox>
                <ElCheckbox v-model="h5config.removes.embed">embed</ElCheckbox>
                <ElCheckbox v-model="h5config.removes.object">object</ElCheckbox>
            </div>
            <div class="MyConfigRow" v-if="mode === 'html5'">
                <span style="margin-right: 0.5em;">Custom rule</span>
                <ElInput v-model="h5config.custom_rule" placeholder="Input custom CSS selector here..." />
            </div>
        </div>
        <div class="MySegment MyButtonGroup">
            <ElButton :type="(!!userContent) ? 'info' : 'primary'" plain class="MyButton MyFlex1" @click="doPaste" :disabled="!!userContent">Paste</ElButton>
            <ElButton type="success" plain class="MyButton MyFlex1" @click="run" :disabled="isRunning || ((mode === 'dompurify') && (!dpConfigValid))">{{ mode_text }}</ElButton>
            <ElButton :type="(!userContent) ? 'info' : 'primary'" plain class="MyButton MyFlex1" @click="doCopy">Copy</ElButton>
            <ElButton type="warning" plain class="MyButton MyFlex1" @click="doUndoChanges" v-show="null !== userContent_old">Undo</ElButton>
            <ElButton type="danger" plain class="MyButton MyFlex1" @click="userContent = ''">Clear</ElButton>
        </div>

        <dialog ref="dpConfigDlg" class="MyFullscreenDialog" @close="saveDPConfig">
            <form method="dialog" style="flex: 1; display: flex; flex-direction: column;" @submit="dpConfigDlg.close()">
                <h3 style="margin-top: 0;">DOMPurify Configuration</h3>
                <div style="flex: 1; display: flex; flex-direction: column;">
                    <ElInput v-model="dpConfig" placeholder="Input DOMPurify configuration here..." type="textarea" class="MyFlex1 MyInput" />
                </div>
                <button class="el-button" type="submit" style="margin-top: 1em;">Save</button>
            </form>
        </dialog>

        <app-loading v-if="isRunning"><load-circle></load-circle></app-loading>
    </div>
</template>

<script setup>
import { ElMessage, ElMessageBox, ElInput } from 'element-plus';
import { computed, onBeforeMount, ref, watch } from 'vue';

const userContent = ref('');
const allowed_modes = ref(['html5', 'dompurify', 'text']);
const MouseEvent = ref(globalThis.MouseEvent)
const dpConfigDlg = ref(null);

function toggleDark() {
    document.documentElement.classList.toggle('dark');
    const MyKey = 'MyHTMLCleanupApp::MyDark';
    if (localStorage.getItem(MyKey) === 'true') {
        localStorage.setItem(MyKey, 'false');
    } else {
        localStorage.setItem(MyKey, 'true');
    }
}

onBeforeMount(() => {
    const dk = localStorage.getItem('MyHTMLCleanupApp::MyDark');
    if (dk === 'true') {
        document.documentElement.classList.add('dark');
    } else if ('false' !== dk) {
        if (window.matchMedia && window.matchMedia('(prefers-color-scheme: dark)').matches) {
            toggleDark();
        }
    }
    const md = localStorage.getItem('MyHTMLCleanupApp::Mode');
    if (allowed_modes.value.includes(md)) {
        mode.value = md;
    }
    const h5conf = localStorage.getItem('MyHTMLCleanupApp::H5Config');
    try {
        const json = JSON.parse(h5conf);
        if (!json) throw json;
        json.removes = json.removes || {};
        h5config.value = json;
    } catch {
        h5config.value = {
            removes: {},
            remove_comments: true,
        };
        const ls = 'style,link,svg,script,meta'.split(',');
        for (const k of ls) {
            h5config.value.removes[k] = true;
        }
    }
    const dpconf = localStorage.getItem('MyHTMLCleanupApp::DPConfig');
    if (dpconf) {
        dpConfig.value = dpconf;
    }
})

function doPaste() {
    navigator.clipboard.readText().then((text) => {
        userContent.value = text;
    }).catch((err) => {
        ElMessage.error('Failed to read clipboard contents: ' + err);
    });
}
function doCopy() {
    navigator.clipboard.writeText(userContent.value).then(() => {
        ElMessage.success('Copied to clipboard');
    }).catch((err) => {
        ElMessage.error('Failed to copy to clipboard:' + err);
    });
}
async function resetPrefs() {
    try {
        await ElMessageBox.confirm('Are you sure you want to reset your preferences?', 'Reset Preferences', {
            confirmButtonText: 'Reset preferences',
            cancelButtonText: 'Cancel',
            type: 'warning',
        });
    } catch { return }
    localStorage.removeItem('MyHTMLCleanupApp::Mode');
    localStorage.removeItem('MyHTMLCleanupApp::H5Config');
    localStorage.removeItem('MyHTMLCleanupApp::DPConfig');
    localStorage.removeItem('MyHTMLCleanupApp::UserSeenH5Warning');
    localStorage.removeItem('MyHTMLCleanupApp::MyDark');
    ElMessageBox.alert('Preferences reset. Please reload or close the page.', 'Reset Preferences', {
        confirmButtonText: 'Refresh',
        type: 'success',
    }).finally(() => {
        window.location.reload();
    })
}

// Core logic
const mode = ref('html5');
const mode_text = computed(() => {
    switch (mode.value) {
        case 'html5': return 'Cleanup';
        case 'dompurify': return 'Purify';
        case 'text': return 'Extract';
        default: return 'ERROR_TYPE';
    }
});
const dpConfig = ref('{}');
const dpConfigValid = computed(() => {
    try {
        JSON.parse(dpConfig.value);
        return true;
    } catch {
        return false;
    }
});
const h5config = ref({
    removes: {},
});
const isRunning = ref(false);
const userContent_old = ref(null);
watch(() => mode.value, (newVal) => {
    localStorage.setItem('MyHTMLCleanupApp::Mode', newVal);
});
watch(() => h5config.value, (newVal) => {
    localStorage.setItem('MyHTMLCleanupApp::H5Config', JSON.stringify(newVal));
}, { deep: true });
const saveDPConfig = () => {
    localStorage.setItem('MyHTMLCleanupApp::DPConfig', dpConfig.value);
};

const userSeenH5Warning_tmpref = ref(false);
const userSeenH5Warning = computed({
    get() {
        userSeenH5Warning_tmpref.value; // To trigger the watcher
        return localStorage.getItem('MyHTMLCleanupApp::UserSeenH5Warning') === 'true';
    },
    set(val) {
        localStorage.setItem('MyHTMLCleanupApp::UserSeenH5Warning', val ? 'true' : 'false');
        userSeenH5Warning_tmpref.value = Date.now(); // To trigger a flush
        return true;
    },
});

function doUndoChanges() {
    if (userContent_old.value === null) {
        ElMessage.warning('There is no changes to undo.');
        return;
    }
    userContent.value = userContent_old.value;
    userContent_old.value = null;
    ElMessage.success('Undone.');
}

async function run() {
    if (isRunning.value || ((mode.value === 'dompurify') && (!dpConfigValid.value))) {
        return;
    }
    if (!allowed_modes.value.includes(mode.value)) {
        ElMessage.error('Invalid mode.');
        return;
    }
    isRunning.value = true;
    const oldLength = userContent.value.length;
    if (mode.value === 'dompurify') try {
        const { default: DOMPurify } = await import('dompurify');
        const config = JSON.parse(dpConfig.value);
        const clean = DOMPurify.sanitize(userContent.value, config);
        userContent_old.value = userContent.value;
        userContent.value = clean;
        ElMessage.success(`Cleaned ${oldLength - userContent.value.length} characters`);
        saveDPConfig();
    } catch (err) {
        ElMessage.error('Failed to run DOMPurify: ' + err);
    } finally {
        isRunning.value = false;
        return;
    }

    // use DOM API to cleanup HTML
    const parser = new DOMParser();
    const doc = parser.parseFromString(userContent.value, 'text/html');
    const root = doc.body;
    root.innerHTML = userContent.value;
    if (mode.value === 'text') {
        userContent_old.value = userContent.value;
        userContent.value = root.textContent;
        ElMessage.success(`Cleaned ${oldLength - userContent.value.length} characters`);
        isRunning.value = false;
        return;
    }
    try {
        // Remove things according to the configuration
        let removesList = Reflect.ownKeys(h5config.value.removes).filter((k) => !!h5config.value.removes[k]).join(',');
        if (h5config.value.custom_rule) {
            removesList += ',' + h5config.value.custom_rule;
        }
        if (removesList) {
            for (const el of root.querySelectorAll(removesList)) {
                el.remove();
            }
        }
        // Remove comments
        if (h5config.value.remove_comments) {
            const walker = document.createTreeWalker(root, NodeFilter.SHOW_COMMENT, null, false);
            const comments = [];
            while (walker.nextNode()) {
                comments.push(walker.currentNode);
            }
            for (const comment of comments) {
                comment.remove();
            }
        }
        // Remove style attribute
        if (h5config.value.remove_style_attr) {
            for (const el of root.querySelectorAll('[style]')) {
                el.removeAttribute('style');
            }
        }
        // Write back to userContent
        userContent_old.value = userContent.value;
        userContent.value = root.innerHTML;
        ElMessage.success(`Cleaned ${oldLength - userContent.value.length} characters`);
    } catch (err) {
        ElMessage.error('Failed to run HTML API: '+ err);
    } finally {
        isRunning.value = false;
    }
}
</script>

<style scoped>
.MyMain {
    display: flex;
    flex-direction: column;
    overflow: hidden;
    flex: 1;
    padding: 10px;
    background-color: var(--el-bg-color);
}
.MyTitle {
    margin-top: 0;
    margin-bottom: 10px;
    display: flex;
    flex-direction: row;
    align-items: center;
    overflow: hidden;
    text-overflow: ellipsis;
    white-space: nowrap;
}
.MyTitle > span {
    font-size: xx-large;
    font-weight: bold;
    flex: 1;
    text-align: center;
    overflow: hidden;
    text-overflow: ellipsis;
    white-space: nowrap;
    margin: 0 0.5em;
}
.MyInput {
    flex: 1;
    display: flex;
    flex-direction: column;
}
.MyInput ::v-deep(textarea) {
    resize: none;
    flex: 1;
}
.MySegment:not(:empty) {
    margin-top: 0.5em;
}
.MyButtonGroup {
    display: flex;
}
.MyFlex1 {
    flex: 1;
}
.MyStat {
    overflow: hidden;
}
.MyScrollControlParent {
    overflow: auto;
}
.MyHideScroll, .MyHideScroll * {
    white-space: nowrap;
    flex-wrap: nowrap;
}
.MyHideScroll::-webkit-scrollbar {
    width: 0;
    height: 0;
}
.MyConfig {
    display: flex;
    flex-direction: column;
}
.MyConfigRow {
    display: flex;
    flex-direction: row;
    align-items: center;
    white-space: nowrap;
}
.MyNoMargin {
    margin: 0;
}
.MyConfig ::v-deep(.el-checkbox) {
    margin-right: 1em;
}
.MyFullscreenDialog {
    width: 100%;
    height: 100%;
    border: none;
}
.MyFullscreenDialog[open] {
    display: flex;
    flex-direction: column;
}
</style>

<style>
:root:not(.dark) app-loading>load-circle::after {
    border-color: #ddd;
    border-top-color: gray;
}
</style>