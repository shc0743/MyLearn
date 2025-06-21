<template>
    <resizable-widget class="app" ref="appRoot" @dragover="set_drop_effect" @drop="e => import_context(1, e)">
        <div slot="widget-caption">AI 绘图</div>
        <div class="container">
            <div class="floating-tool">
                <button @click="codeShowingDialog.open = true">显示代码</button>
                <button @click="save_img">保存图片</button>
                <button @click="moreOptDialog.showModal()">更多选项</button>
                <button @click="cmdExecuteDialog.open = true">手动绘制</button>
                <button @click="overlayMode = (1 - overlayMode)">{{ overlayMode ? '叠加' : '覆盖' }}模式</button>
            </div>
            <resizable-widget ref="codeShowingDialog" style="width: 60%; height: 60%;">
                <div slot="widget-caption">代码</div>
                <div class="myCodeDialog" open>
                    <textarea autocomplete="off" v-model="currentCodeShowToUser"></textarea>
                    <button @click="codeShowingDialog.close()" class="MyButton">关闭</button>
                </div>
            </resizable-widget>
            <dialog ref="promptDialog" class="myCodeDialog">
                <textarea autocomplete="off" v-model="prompt"></textarea>
                <button @click="promptDialog.close()" class="MyButton">关闭</button>
            </dialog>
            <resizable-widget ref="cmdExecuteDialog" style="width: 50%; height: 50%; --padding: 10px;">
                <div slot="widget-caption">绘制</div>
                <div style="display: flex; flex-direction: column; width: 100%; height: 100%;">
                    <textarea autocomplete="off" v-model="cmdToExecute" style="flex: 1; border: 0; margin-bottom: 0.5em; resize: none; box-sizing: border-box; font-size: 1rem;"></textarea>
                    <button @click="(userCmdExec(), cmdExecuteDialog.close())" class="MyButton">执行</button>
                </div>
            </resizable-widget>
            <dialog ref="moreOptDialog" class="myCodeDialog" style="width: 300px; height: min-content;">
                <button class="MyButton" @click="promptDialog.showModal()">修改系统提示</button>
                <button class="MyButton" @click="export_context">导出上下文</button>
                <button class="MyButton" @click="import_image">导入图片</button>
                <button class="MyButton" @click="reload_canvas">重新加载画布</button>
                <button class="MyButton" @click="overlayMode = (1 - overlayMode)">{{ overlayMode ? '叠加' : '覆盖' }}模式</button>
                <hr>
                <label><input type="checkbox" autocomplete="off" v-model="fit_img_to_canvas">&nbsp;图片导入：适应画布大小</label>
                <hr>
                <button @click="moreOptDialog.close()" class="MyButton">关闭</button>
            </dialog>
            <div class="draw-area">
                <iframe src="./usercontent.html" ref="myFrame" sandbox="allow-scripts"></iframe>
            </div>
            <div class="user-communicate-area">
                <div v-show="manageKey" class="backdrop"></div>
                <div v-show="manageKey" class="api-key-view">
                    <div class="api-key-card">
                        <h3>API Key 管理</h3>
                        <div class="input-group">
                            <input type="text" @keydown.enter="saveApikey" autocomplete="off" v-model="apikey"
                                placeholder="请输入您的 API Key" class="api-input">
                            <label><input type="checkbox" autocomplete="off" v-model="rememberKey"> 记住 API Key</label>
                        </div>
                        <button @click="saveApikey" class="MyButton">保存设置</button>
                    </div>
                </div>
                <input type="file" @change="import_context(0)" ref="hiddenFileCtxImportField" hidden>

                <div style="padding: 10px;">
                    <a href="javascript:" @click.prevent="manageKey = true">设置 API Key</a>
                    <span>&nbsp;|&nbsp;</span>
                    <a href="javascript:" @click.prevent="((context.length = 0), (currentFailures.length = 0))">清空上下文</a>
                    <span>&nbsp;|&nbsp;</span>
                    <label><input type="checkbox" autocomplete="off" v-model="deep_think">&nbsp;深度思考</label>
                </div>

                <div class="message-area" @wheel.passive="userScrolled = true" @pointerdown="userScrolled = true"
                    ref="msgArea">
                    <div v-for="(item, index) in context" :key="index" :class="['message-bubble', 'role-' + item.role]">
                        <div class="message-header">
                            <span class="message-role">{{ item.role === 'user' ? '用户' : 'AI' }}</span>
                            <span class="message-time">{{ formatTime(item.timestamp) }}</span>
                        </div>
                        <div v-if="!!item.reasoning_content" class="message-reasoning-content">{{ item.reasoning_content
                            }}</div>
                        <div class="message-content">{{ item.content }}</div>
                    </div>

                    <div v-if="context.length === 0" style="display: flex; flex-direction: column; align-items: center; word-break: break-all; text-align: center;">
                        <div style="font-weight: bold; font-size: 1.5rem; margin-bottom: 0.5em;">上下文是空的！</div>
                        <div>可以<a href="javascript:" @click="hiddenFileCtxImportField.click()">导入上下文</a>，也可以直接<br>拖拽上下文文件到此。</div>
                    </div>

                    <div v-if="currentResponse.reasoning_content || currentResponse.content"
                        class="message-bubble role-assistant">
                        <div class="message-header">
                            <span class="message-role">AI</span>
                            <span class="message-time">{{ formatTime(Date.now()) }}</span>
                        </div>
                        <div v-if="currentResponse.reasoning_content" class="message-reasoning-content">{{
                            currentResponse.reasoning_content }}</div>
                        <div class="message-content">{{ currentResponse.content }}</div>
                    </div>
                    
                    <div v-if="currentFailures.length > 0" class="message-bubble role-error">
                        <div class="message-header">
                            <span class="message-role">错误</span>
                            <span class="message-time">{{ formatTime(Date.now()) }}</span>
                        </div>
                        <div class="message-content">
                            <div v-for="(failure, index) in currentFailures" :key="index" class="failure-item">
                                {{ index + 1 }}. {{ failure }}
                            </div>
                        </div>
                        <div style="margin-top: 0.5em;"><a href="javascript:" @click="aiFixErrors" aria-label="通过 AI 修复代码中的错误。">AI 修复</a></div>
                    </div>
                </div>

                <div class="input-msg-area">
                    <textarea autocomplete="off" v-model="inputMsg" placeholder="请输入您的消息"
                        @keydown.enter="handleEnterDown"></textarea>
                    <button class="MyButton" type="button" @click="isGeneratingResponse ? abort() : send()">
                        {{ isGeneratingResponse ? '停止' : '发送' }}
                    </button>
                </div>
            </div>
        </div>
    </resizable-widget>
</template>

<script setup>
import { computed, nextTick, onMounted, ref, watch } from 'vue';
import { toast } from 'vue3-toastify';

const appRoot = ref(null);

onMounted(async () => {
    appRoot.value.style.width = window.innerWidth + 'px';
    appRoot.value.style.height = window.innerHeight + 'px';
    apikey.value = localStorage.getItem('my-ai-canvas-painting::apikey') || '';
    if (!apikey.value) {
        manageKey.value = true;
    }
    prompt.value = localStorage.getItem('my-ai-canvas-painting::prompt') || await(fetch('./prompt.txt').then(r => r.text()));
    import('@microsoft/fetch-event-source') // 预加载模块以提高性能
})

const myFrame = ref(null);

const apikey = ref('');
const manageKey = ref(false);
const rememberKey = ref(true);
const saveApikey = () => {
    if (!apikey.value) {
        localStorage.removeItem('my-ai-canvas-painting::apikey');
        manageKey.value = false;
        return;
    }
    if (rememberKey.value) {
        localStorage.setItem('my-ai-canvas-painting::apikey', apikey.value);
    } else {
        localStorage.removeItem('my-ai-canvas-painting::apikey');
    }
    manageKey.value = false;
}

const inputMsg = ref('');
const deep_think = ref(false);
const context = ref([]);
const prompt = ref('');
const overlayMode = ref(0);

function formatTime(timestamp) {
    if (!timestamp) return '';
    const date = new Date(timestamp);
    return `${date.getHours()}:${date.getMinutes().toString().padStart(2, '0')}:${date.getSeconds().toString().padStart(2, '0')}`;
}

const msgArea = ref(null);
const codeShowingDialog = ref(null);
const promptDialog = ref(null);
const moreOptDialog = ref(null); 
const cmdExecuteDialog = ref(null);
const cmdToExecute = ref('');
const hiddenFileCtxImportField = ref(null);
const fit_img_to_canvas = ref(false);
const dl = (data, filext = '.txt', filename = `AI 绘图 - ${(new Date()).toLocaleString()}`) => {
    let url = data;
    if (typeof data === 'object' && data instanceof Blob) {
        url = URL.createObjectURL(data);
    }
    const a = document.createElement('a');
    a.href = url;
    a.download = filename + filext;
    a.target = '_blank';
    document.body.append(a);
    a.click();
    a.remove();
    if (typeof data === 'object' && data instanceof Blob) {
        URL.revokeObjectURL(url);
    }
}
const save_img = () => {
    execCommandInContext('get_data').then(dataUrl => {
        dl(dataUrl, '.png');
        toast.success('图片已保存');
    }).catch(e => {
        toast.error(e);
    });
}
const export_context = () => {
    const ctx = JSON.stringify(context.value, null, 2);
    dl(new Blob([ctx]), '.json');
}
const reload_canvas = () => {
    myFrame.value.src = myFrame.value.src;
    toast.success('画布已重新加载')
    moreOptDialog.value.close();
}
const import_image = () => {
    hiddenFileCtxImportField.value.click();
}
const import_context = async (src, e) => {
    let file;
    if (src === 0) {
        file = hiddenFileCtxImportField.value.files[0];
    }
    if (src === 1) {
        file = e.dataTransfer.files[0];
        if (file) e.preventDefault();
    }
    if (!file) return;
    if (file?.name?.endsWith?.('.png') || file?.name?.endsWith?.('.jpg') || file?.name?.endsWith?.('.jpeg') || file?.name?.endsWith?.('.webp')) {
        // 绘制到 canvas 上
        const reader = new FileReader();
        reader.readAsDataURL(file);
        reader.onload = async () => {
            try {
                await execCommandInContext('clear');
                const { success, error } = await execCommandInContext('draw_image', 'command', {
                    url: reader.result,
                    fit: fit_img_to_canvas.value,
                });
                if (success) toast.success('图片导入成功');
                else throw ('执行失败，原因：' + error);
            } catch (e) {
                toast.error(e);
            }
        }
        return;
    }
    if (file?.name?.endsWith?.('.json')) try {
        // 导入对话数据
        if (context.value.length) throw '当前对话数据不为空，无法导入；请先清空对话数据';
        const ctx = await file.text();
        context.value = JSON.parse(ctx);
        toast.success('对话数据已导入');
        return;
    } catch (error) {
        toast.error('导入对话数据失败：' + error);
        return;
    }
    return toast.error('不知道怎么处理这样的文件: ' + file.name);
}
const set_drop_effect = e => {
    if (!e.dataTransfer.items.length || e.dataTransfer.items[0].kind !== 'file') return;
    e.preventDefault();
    e.dataTransfer.dropEffect = 'link';
}
const userCmdExec = () => {
    execCommandInContext(cmdToExecute.value, 'code').then(r => {
        if (r.success) toast.success('已执行，结果：' + JSON.stringify(r));
        else toast.error('执行失败，原因：' + r.error);
    }).catch(e => {
        toast.error(e);
    }).finally(() => {
        cmdToExecute.value = '';
    });
}
const handleEnterDown = e => {
    if (!e.shiftKey) { e.preventDefault(); send(); }
};
watch(() => prompt.value, (newValue) => {
    localStorage.setItem('my-ai-canvas-painting::prompt', newValue);
});

async function execCommandInContext(cmd, type='command', data=null) {
    const ctx = myFrame.value.contentWindow;
    const sender = crypto.randomUUID();
    return await new Promise((resolve, reject) => {
        const func = e => {
            if (e.origin !== location.origin && e.origin !== 'null') return; 
            if (e.data.sender !== sender) return;
            resolve(e.data.data);
            window.removeEventListener('message', func);
        };
        window.addEventListener('message', func);
        ctx.postMessage({
            [type]: cmd,
            data,
            sender,
        }, '*');
        setTimeout(() => {
            window.removeEventListener('message', func);
            reject('timeout');
        }, 10000);
    });
}

async function constructDeepSeekContext() {
    const info = await execCommandInContext('getinfo');
    const messages = [{
        role: 'system',
        content: prompt.value.replace(/(%canvas_width|%canvas_height|%current_time)/g, (m) => {
            if (m === '%canvas_width') return info.width;
            if (m === '%canvas_height') return info.height;
            if (m === '%current_time') return (new Date()).toISOString();
        }),
    }];
    for (const i of context.value) {
        if (i.role === 'user') {
            messages.push({
                role: 'user',
                content: i.content,
            });
        } else {
            messages.push({
                role: 'assistant',
                content: i.content,
            });
        }
    }

    return {
        messages,
        model: deep_think.value ? 'deepseek-reasoner' : 'deepseek-chat',
        stream: true,
        max_tokens: 4096,
    }
}

const currentResponse = ref({ reasoning_content: '', content: '' });
const currentResponseContent = ref('');
const currentCode = ref('');
const currentFailures = ref([]);
const isGeneratingResponse = ref(false);
const abortController = ref(null);
const userScrolled = ref(false)
const currentCodeShowToUser = computed({ get: () => currentCode.value, set: () => false });

function parse() {
    // 功能：解析网络请求
    if (!currentResponseContent.value) return;

    // 首先，检查响应中是否有我们要的```image代码块
    const imageCodeBlock = currentResponseContent.value.indexOf("```image");
    if (-1 === imageCodeBlock) return;

    // 然后，检查是否是一个完整的响应（含有 ``` 结束符）或者部分完整的响应（在 ```image 后面含有至少一个 \n ）
    const endCodeBlock = currentResponseContent.value.indexOf("```", imageCodeBlock + 9);
    const lineBreak = currentResponseContent.value.indexOf("\n", imageCodeBlock + 9);

    if (-1 === endCodeBlock && -1 === lineBreak) return;

    // 提取字符串。
    let cmd;
    if (endCodeBlock !== -1) {
        cmd = currentResponseContent.value.substring(imageCodeBlock + 9, endCodeBlock);
    } else {
        cmd = currentResponseContent.value.substring(imageCodeBlock + 9, lineBreak);
    }
    if (!cmd) return; // 提取出来的是空内容，说明是不完整的响应，不处理。
    currentCode.value += cmd + '\n';

    // 移除原来的字符串中这一部分，防止重复处理。
    const removalEnd = endCodeBlock !== -1 ? endCodeBlock + 3 : lineBreak + 1;
    if (endCodeBlock !== -1) {
        // 完整的响应，移除 ``` 结束符。
        console.log('endCodeBlock:', endCodeBlock);
        currentResponseContent.value = currentResponseContent.value.substring(0, imageCodeBlock) + currentResponseContent.value.substring(removalEnd);
    } else {
        // 保留 ```image 部分，移除 \n 后面的内容。
        currentResponseContent.value = currentResponseContent.value.substring(0, imageCodeBlock + 9) + currentResponseContent.value.substring(removalEnd);
    }

    // 执行命令
    execCommandInContext(cmd, 'code').then(({ success, error, stack }) => {
        if (!success) {
            currentFailures.value.push(stack || error);
        }
    }).catch(e => {
        toast.error('命令执行失败: ' + e);
    })
}
function abort() {
    abortController.value?.abort();
    if (abortController.value) {
        toast.info("已停止生成");
        abortController.value = null;
    }
}
async function send() {
    if (!inputMsg.value || isGeneratingResponse.value) {
        return;
    }
    
    isGeneratingResponse.value = true;
    abortController.value = new AbortController();
    currentResponse.value = { reasoning_content: '', content: '' };
    context.value.push({
        role: 'user',
        content: inputMsg.value,
        timestamp: Date.now()
    });
    inputMsg.value = '';
    currentCode.value = currentResponseContent.value = '';
    currentFailures.value.length = 0;
    userScrolled.value = false;

    await new Promise(nextTick);
    msgArea.value.scrollTop = msgArea.value.scrollHeight;
    toast.success("正在生成...");

    try {
        const { fetchEventSource } = await import('@microsoft/fetch-event-source');
        
        // 先清屏
        if (overlayMode.value === 0) await execCommandInContext('clear');
        let isFirstChunk = true;
        
        await fetchEventSource('https://api.deepseek.com/chat/completions', {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json',
                'Authorization': 'Bearer ' + apikey.value,
            },
            body: JSON.stringify(await constructDeepSeekContext()),
            onmessage: (event) => {
                if (event.data === '[DONE]' || !event.data) {
                    return;
                }
                try {
                    if (isFirstChunk) {
                        userScrolled.value = false;
                        isFirstChunk = false;
                    }
                    const data = JSON.parse(event.data);
                    if (data.choices[0].delta.reasoning_content) {
                        currentResponse.value.reasoning_content += data.choices[0].delta.reasoning_content;
                    }
                    if (data.choices[0].delta.content) {
                        currentResponse.value.content += data.choices[0].delta.content;
                        currentResponseContent.value += data.choices[0].delta.content;
                    }
                    if (data.choices[0].delta.content) parse();
                    if (!userScrolled.value) {
                        msgArea.value.scrollTop = msgArea.value.scrollHeight;
                    }
                } catch (e) {
                    console.error('[app]', 'Error parsing SSE data:', e);
                }
            },
            onerror: e => { throw e },
            signal: abortController.value.signal
        });

        // 完成后，把解析后的字符串添加到上下文。
        context.value.push({
            role: 'assistant',
            content: currentResponse.value.content,
            reasoning_content: currentResponse.value.reasoning_content,
            timestamp: Date.now()
        });
        currentResponse.value = { reasoning_content: '', content: '' };
        await new Promise(nextTick);
        if (!userScrolled.value) {
            msgArea.value.scrollTop = msgArea.value.scrollHeight;
        }
    } catch (e) {
        if (e?.name !== 'AbortError') {
            console.error('[app]', 'Error fetching SSE:', e);
            toast.error("生成失败！" + e);
        }
    } finally {
        isGeneratingResponse.value = false;
        abortController.value = null;
    }
}
async function aiFixErrors() {
    const errorsStr = currentFailures.value.join('\n\n');
    if (!errorsStr) {
        toast.info("没有错误信息");
        return;
    }
    if (isGeneratingResponse.value) {
        toast.info("暂不可用，请等待生成完成");
        return;
    }
    const oldInputMsg = inputMsg.value;
    inputMsg.value = `你刚才提供的代码有错误，请修复这些错误:\n\n\`\`\`js\n${errorsStr}\n\`\`\`\n\n请修复错误后重新给出正确的、能够正常运行的代码。`;
    send();
    await new Promise(nextTick);
    inputMsg.value = oldInputMsg;
}
</script>

<style scoped>
.app {
    display: flex;
    flex-direction: row;
    --padding: 0;
}
.container {
    display: flex;
    flex-direction: row;
    width: 100%; height: 100%;
    box-sizing: border-box;
    overflow: hidden;
}
.floating-tool {
    position: absolute;
    bottom: 20px;
    left: 20px;
    z-index: 1;
}
.floating-tool > button {
    background-color: #fff;
    border: 1px solid #dcdfe6;
    height: 32px;
    padding: 8px 15px;
    border-radius: 5px;
    transition: .1s;
}
.floating-tool > button + button {
    margin-left: 10px;
}
.floating-tool > button:hover {
    background-color: #f0f0f0;
}
.floating-tool > button:active {
    background-color: #d0d0d0;
}
.draw-area {
    flex: 7;
    border-right: 1px solid #000;
    box-sizing: border-box;
}
.user-communicate-area {
    flex: 3;
}
@media screen and (max-width: 600px) {
    .draw-area {
        flex: 1;
        border-right: 0;
        border-bottom: 1px solid #000;
    }
    .user-communicate-area {
        flex: 1;
    }
    .container {
        flex-direction: column;
    }
}
.draw-area > iframe {
    width: 100%;
    height: 100%;
    border: 0;
    box-sizing: border-box;
}
.user-communicate-area {
    position: relative;
    display: flex;
    flex-direction: column;
    overflow: hidden;
    min-width: 22em;
}
.user-communicate-area > .api-key-view {
    position: absolute;
    left: 50%;
    top: 50%;
    transform: translate(-50%, -50%);
    background: #fff;
    border: 1px solid #000;
    box-sizing: border-box;
    z-index: 2;
}
.user-communicate-area > .backdrop {
    position: absolute;
    inset: 0;
    background: rgba(0, 0, 0, 0.5);
    z-index: 2;
}
.message-area {
    display: flex;
    flex-direction: column;
    flex: 1;
    margin: 0.5em 0;
    padding: 10px;
}
.input-msg-area {
    display: flex;
    flex-direction: row;
    padding: 10px;
}
.input-msg-area > textarea {
    flex: 1;
    resize: none;
    padding: 10px;
    border: 1px solid #ddd;
    border-radius: 6px;
    height: 5em;
    box-sizing: content-box;
}
:focus-visible {
    outline: 2px solid rgb(159.5, 206.5, 255);
    outline-offset: 1px;
    transition: outline-offset 0s, outline 0s;
}
.input-msg-area > button {
    margin-left: 10px;
}
.failure-item {
    background-color: rgb(236, 224, 255);
    padding: 5px;
    border-radius: 10px;
}
.failure-item + .failure-item {
    margin-top: 10px;
}
</style>

<style scoped>
.message-area {
    flex: 1;
    overflow-y: auto;
    padding: 10px;
    display: flex;
    flex-direction: column;
    gap: 12px;
    word-break: break-all;
}

.message-bubble {
    max-width: 80%;
    padding: 12px 16px;
    border-radius: 12px;
    position: relative;
    word-break: break-all;
}

.message-header {
    display: flex;
    justify-content: space-between;
    margin-bottom: 6px;
    font-size: 0.8rem;
    color: #666;
}
.role-user .message-header {
    color: #fff;
}

.role-error {
    border: 1px solid #dd0000;
}

.message-time {
    opacity: 0.7;
    margin-left: 0.5em;
}

.role-user {
    align-self: flex-end;
    background: #4a90e2;
    color: white;
}

.role-assistant {
    align-self: flex-start;
    background: #f0f0f0;
    color: #333;
}

.message-reasoning-content {
    font-size: 0.9rem;
    color: #444;
    margin-bottom: 8px;
    white-space: pre-wrap;
    border-bottom: 1px solid #000;
    padding-bottom: 0.5em;
}

.message-content {
    line-height: 1.5;
    white-space: pre-wrap;
}

.api-key-view {
    display: flex;
    justify-content: center;
    align-items: center;
    z-index: 100;
}

.MyButton {
    background-color: rgb(235.9, 245.3, 255);
    color: white;
    padding: 10px 20px;
    border: 1px solid rgb(159.5, 206.5, 255);
    border-radius: 5px;
    cursor: pointer;
    font-size: 16px;
    transition: all .3s;
    color: #409eff;
}
.MyButton:hover {
    background-color: #409eff;
    border-color: #409eff;
    color: #fff;
}
.MyButton:active {
    background-color: #337ecc;
}

.api-key-card {
    background: white;
    padding: 2rem;
    border-radius: 12px;
    box-shadow: 0 4px 20px rgba(0, 0, 0, 0.15);
    max-width: 90%;
}

.api-key-card h3 {
    margin: 0 0 1.5rem 0;
    font-size: 1.5rem;
    color: #333;
    text-align: center;
}

.input-group {
    margin-bottom: 1.5rem;
    display: flex;
    flex-direction: column;
}

.api-input {
    width: 100%;
    padding: 12px;
    border: 1px solid #ddd;
    border-radius: 6px;
    font-size: 1rem;
    transition: border-color 0.3s;
    box-sizing: border-box;
}

.api-input:focus {
    outline: none;
    border-color: #4a90e2;
}

.myCodeDialog {
    display: flex;
    flex-direction: column;
    width: 100%;
    height: 100%;
}
.myCodeDialog:not([open]) {
    display: none !important;
}
.myCodeDialog > textarea {
    flex: 1;
    font-size: 1rem;
    padding: 10px;
    border-radius: 10px;
}
.myCodeDialog > button {
    margin-top: 0.5em;
}
</style>