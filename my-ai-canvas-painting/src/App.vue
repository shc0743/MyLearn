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
                    <textarea autocomplete="off" v-model="currentCodeShowToUser" style="border: 0;"></textarea>
                    <button @click="codeShowingDialog.close()" class="MyButton">关闭</button>
                </div>
            </resizable-widget>
            <dialog ref="promptDialog" class="myCodeDialog">
                <textarea autocomplete="off" v-model="prompt"></textarea>
                <button @click="promptDialog.close()" class="MyButton">关闭</button>
            </dialog>
            <dialog ref="userParseSystemPromptDialog" class="myCodeDialog">
                <textarea autocomplete="off" :value="parsed_content2" readonly></textarea>
                <button @click="userParseSystemPromptDialog.close()" class="MyButton">关闭</button>
            </dialog>
            <resizable-widget ref="cmdExecuteDialog" style="width: 50%; height: 50%; --padding: 10px;">
                <div slot="widget-caption">绘制</div>
                <div style="display: flex; flex-direction: column; width: 100%; height: 100%;">
                    <textarea autocomplete="off" v-model="cmdToExecute"
                        style="flex: 1; border: 0; margin-bottom: 0.5em; resize: none; box-sizing: border-box; font-size: 1rem;"></textarea>
                    <button @click="(userCmdExec(), cmdExecuteDialog.close())" class="MyButton">执行</button>
                </div>
            </resizable-widget>
            <dialog ref="moreOptDialog" class="myCodeDialog" style="width: 300px; height: min-content;">
                <button class="MyButton" @click="promptDialog.showModal()">修改系统提示</button>
                <button class="MyButton"
                    @click="((userParseSystemPromptDialog.showModal()), (parsed_content().then(v => parsed_content2 = v)))">解析系统提示</button>
                <button class="MyButton" @click="export_context">导出上下文</button>
                <button class="MyButton" @click="import_image">导入图片</button>
                <button class="MyButton" @click="reload_canvas">重新加载画布</button>
                <button class="MyButton" @click="overlayMode = (1 - overlayMode)">{{ overlayMode ? '叠加' : '覆盖'
                    }}模式</button>
                <hr>
                <label><input type="checkbox" autocomplete="off" v-model="auto_scroll">&nbsp;AI 聊天：自动滚动</label>
                <label><input type="checkbox" autocomplete="off" v-model="oneline_mode">&nbsp;AI 聊天：单行模式（逐步执行代码）</label>
                <label><input type="checkbox" autocomplete="off" v-model="try_autorerun">&nbsp;AI 聊天：自动重试</label>
                <label><input type="checkbox" autocomplete="off" v-model="fit_img_to_canvas">&nbsp;图片导入：适应画布大小</label>
                <label><input type="checkbox" autocomplete="off"
                        v-model="check_for_prompt_update">&nbsp;系统：检查提示词更新</label>
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
                    <a href="javascript:"
                        @click.prevent="((context.length = 0), (currentFailures.length = 0))">清空上下文</a>
                    <span>&nbsp;|&nbsp;</span>
                    <label><input type="checkbox" autocomplete="off" v-model="deep_think">&nbsp;深度思考</label>
                </div>

                <div class="message-area" @wheel.passive="userScrolled = true" @pointerdown="userScrolled = true"
                    ref="msgArea">
                    <div v-for="(item, index) in context" :key="index" class="message-bubble" :data-role="item.role"
                        @draw="drawCmd(item, $event.detail)" @viewsource="viewCmdSource(item, $event)">
                        <div class="message-header">
                            <span class="message-role">{{ item.role === 'user' ? '用户' : 'AI' }}</span>
                            <span class="message-time">{{ formatTime(item.timestamp) }}</span>
                        </div>
                        <div v-if="!!item.reasoning_content" class="message-reasoning-content"
                            v-text="item.reasoning_content"></div>
                        <div class="message-content" v-for="(line, index) in render_lines(item.content)" :key="index"
                            v-html="p(line)"></div>
                    </div>

                    <div v-if="context.length === 0"
                        style="display: flex; flex-direction: column; align-items: center; word-break: break-all; text-align: center;">
                        <div style="font-weight: bold; font-size: 1.5rem; margin-bottom: 0.5em;">上下文是空的！</div>
                        <div>可以<a href="javascript:"
                                @click="hiddenFileCtxImportField.click()">导入上下文</a>，也可以直接<br>拖拽上下文文件到此。</div>
                    </div>

                    <div v-if="currentResponse.reasoning_content || currentResponse.content" class="message-bubble"
                        data-role="assistant"
                        @draw="drawCmd(currentResponse, $event.detail)" 
                        @viewsource="viewCmdSource(currentResponse, $event)"
                    >
                        <div class="message-header">
                            <span class="message-role">AI</span>
                            <span class="message-time">{{ formatTime(Date.now()) }}</span>
                        </div>
                        <div v-if="currentResponse.reasoning_content" class="message-reasoning-content"
                            v-text="currentResponse.reasoning_content"></div>
                        <div class="message-content" v-for="(line, index) in render_lines(currentResponse.content)"
                            :key="index" v-html="p(line)"></div>
                    </div>

                    <div v-if="currentFailures.length > 0" class="message-bubble role-error">
                        <div class="message-header">
                            <span class="message-role">错误 ({{ currentFailures.length }})</span>
                            <span class="message-time">{{ formatTime(Date.now()) }}</span>
                        </div>
                        <div class="message-content">
                            <div v-for="(failure, index) in currentFailures" :key="index" class="failure-item">
                                {{ index + 1 }}. {{ failure }}
                            </div>
                        </div>
                        <div style="margin-top: 0.5em;" class="my-links">
                            <a href="javascript:" @click="rerunErrors" title="尝试重新运行对应代码，观察错误是否消失。" v-if="!isGeneratingResponse && currentFailedIndex">重新运行</a>
                            <a href="javascript:" @click="aiFixErrors" title="通过 AI 修复代码中的错误。" v-if="!isGeneratingResponse">AI修复</a>
                            <a href="javascript:" @click="((currentFailures.length = 0), (currentFailedIndex = 0))" title="清除错误列表。">清除</a>
                        </div>
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
import { parse_template_string } from './parse_template_string';
import DOMPurify from 'dompurify';

const appRoot = ref(null);
const p = (html) => DOMPurify.sanitize(html, {
    ADD_TAGS: ["x-canvas-api-draw-im-command"],
    ALLOW_DATA_ATTR: true,
});

onMounted(async () => {
    appRoot.value.style.width = window.innerWidth + 'px';
    appRoot.value.style.height = window.innerHeight + 'px';
    apikey.value = localStorage.getItem('my-ai-canvas-painting::apikey') || '';
    if (!apikey.value) {
        manageKey.value = true;
    }
    doLoad(auto_scroll, 'auto_scroll', false);
    doLoad(fit_img_to_canvas, 'fit_img_to_canvas', false);
    doLoad(check_for_prompt_update, 'check_for_prompt_update', true);
    doLoad(oneline_mode, 'oneline_mode', true);
    doLoad(overlayMode, 'overlayMode', 0);
    doLoad(try_autorerun, 'try_autorerun', true);
    prompt.value = localStorage.getItem('my-ai-canvas-painting::prompt') || await (fetch('./prompt.txt', {
        cache: 'no-store',
    }).then(r => r.text()));

    import('@microsoft/fetch-event-source') // 预加载模块以提高性能

    // 检查提示词是否更新
    if (check_for_prompt_update.value) {
        const promptVersion = +localStorage.getItem('my-ai-canvas-painting::prompt_version');
        const remote_prompt_version = isNaN(promptVersion) ? 0 : (+(await (await fetch('./prompt_version.txt', {
            cache: 'no-store',
        })).text())); // 节省流量
        if (isNaN(promptVersion) || promptVersion < remote_prompt_version) {
            let userWantsToUpdate = false;
            toast.info('系统提示词有更新！点击进行更新。（如果您修改了系统提示词，更新后修改将被覆盖；这种情况下请点击“x”取消更新）', {
                autoClose: false,
                closeButton: true,
                onClose: async () => {
                    if (!userWantsToUpdate) toast.info('您可以在 “更多选项” 中禁用更新检查。', {
                        onClick() {
                            moreOptDialog.value.showModal();
                        }
                    });
                },
                onClick: () => {
                    userWantsToUpdate = true;
                    queueMicrotask(async () => {
                        prompt.value = await (await fetch('./prompt.txt', {
                            cache: 'no-store',
                        })).text();
                        localStorage.setItem('my-ai-canvas-painting::prompt_version', remote_prompt_version);
                        toast.success('提示词已更新。');
                    });
                }
            })
        }
    }
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
const userParseSystemPromptDialog = ref(null);
const cmdToExecute = ref('');
const hiddenFileCtxImportField = ref(null);
const auto_scroll = ref(false);
const fit_img_to_canvas = ref(false);
const check_for_prompt_update = ref(false);
const oneline_mode = ref(false);
const try_autorerun = ref(false);
const parsed_content2 = ref('');
const temporarilyMultipleLineExecution = ref(false);
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
    const filename = (file?.name || '').toLowerCase();
    if (filename.endsWith('.png') || filename.endsWith('.jpg') || filename.endsWith('.jpeg') || filename.endsWith('.webp')) {
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
    if (filename?.endsWith?.('.json')) try {
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
        else toast.error('执行失败，原因：' + (r.stack || r.error));
    }).catch(e => {
        toast.error(e);
    }).finally(() => {
        cmdToExecute.value = '';
    });
}
const handleEnterDown = e => {
    if (!e.shiftKey) { e.preventDefault(); send(); }
};
const doWatch = (object, object_str) => {
    watch(() => object.value, (newValue) => {
        localStorage.setItem('my-ai-canvas-painting::' + object_str, newValue);
    });
};
const doLoad = (object, object_str, default_Value = false) => {
    const value = localStorage.getItem('my-ai-canvas-painting::' + object_str);
    if (value == null) {
        object.value = default_Value;
    } else {
        object.value = (value === 'true') ? true : (value === 'false') ? false : (
            Number.isNaN(+value) ? value : +value
        );
    }
}
doWatch(prompt, 'prompt');
doWatch(auto_scroll, 'auto_scroll');
doWatch(fit_img_to_canvas, 'fit_img_to_canvas');
doWatch(check_for_prompt_update, 'check_for_prompt_update');
doWatch(oneline_mode, 'oneline_mode');
doWatch(try_autorerun, 'try_autorerun');
doWatch(overlayMode, 'overlayMode');

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

const parsed_content = async () => {
    const info = await execCommandInContext('getinfo');
    return parse_template_string(prompt.value, {
        oneline: !!oneline_mode.value,
        canvas_width: info.width,
        canvas_height: info.height,
        current_time: (new Date()).toISOString(),
    })
};
async function constructDeepSeekContext() {
    const content = await parsed_content();
    const messages = [{
        role: 'system',
        content,
    }];
    for (const i of context.value) {
        if (i.role === 'user') {
            messages.push({
                role: 'user',
                content: i.content,
            });
        } else if (i.role === 'assistant') {
            messages.push({
                role: 'assistant',
                content: i.raw_content || i.content,
            });
        } else {
            toast.warning('数据异常：' + i.role, { delay: 250 });
        }
    }

    return {
        messages,
        model: deep_think.value ? 'deepseek-reasoner' : 'deepseek-chat',
        stream: true,
        max_tokens: 4096,
    }
}

const currentResponse = ref({ id: '', reasoning_content: '', content: '', raw_content: '', nextCmd: 0, commands: {} });
const currentCode = ref('');
const currentFailures = ref([]);
const currentFailedIndex = ref(0);
const isGeneratingResponse = ref(false);
const abortController = ref(null);
const userScrolled = ref(false)
const currentCodeShowToUser = computed({ get: () => currentCode.value, set: () => false });
const previousIsFullResponse = ref(false)

function parse() {
    // 功能：解析网络请求
    if (!currentResponse.value.content) return;

    // 首先，检查响应中是否有我们要的```image代码块
    const imageCodeBlock = currentResponse.value.content.indexOf("```image");
    if (-1 === imageCodeBlock) return;

    // 然后，检查是否是一个完整的响应（含有 ``` 结束符）或者部分完整的响应（在 ```image 后面含有至少一个 \n ）
    const endCodeBlock = currentResponse.value.content.indexOf("```", imageCodeBlock + 9);
    const lineBreak = currentResponse.value.content.indexOf("\n", imageCodeBlock + 9);

    if (-1 === endCodeBlock && -1 === lineBreak) return;

    // 提取字符串。
    let cmd;
    if (endCodeBlock !== -1) {
        cmd = currentResponse.value.content.substring(imageCodeBlock + 9, endCodeBlock);
    } else {
        if (!oneline_mode.value || temporarilyMultipleLineExecution.value) return; // 等待完整响应
        cmd = currentResponse.value.content.substring(imageCodeBlock + 9, lineBreak);
    }
    if (!cmd && endCodeBlock === -1) return; // 提取出来的是空内容，说明是不完整的响应，不处理。
    if (previousIsFullResponse.value) {
        currentCode.value = '';
        previousIsFullResponse.value = false;
    }
    currentCode.value += cmd + '\n';
    // console.log('command: ', cmd);

    // 移除原来的字符串中这一部分，防止重复处理。
    const removalEnd = endCodeBlock !== -1 ? endCodeBlock + 3 : lineBreak + 1;
    let cmdIndex = currentResponse.value.nextCmd;
    if (endCodeBlock !== -1) {
        // 完整的响应，移除 ``` 结束符。
        // console.log('full response', currentResponse.value.content);
        cmdIndex = ++currentResponse.value.nextCmd;
        currentResponse.value.commands[cmdIndex] = currentCode.value;
        currentResponse.value.content =
            currentResponse.value.content.substring(0, imageCodeBlock) +
            '<x-canvas-api-draw-im-command data-entity-id=' + currentResponse.value.id + ' data-cmd-index=' + cmdIndex + '></x-canvas-api-draw-im-command>' +
            currentResponse.value.content.substring(removalEnd);
        // currentCode.value = '';
        previousIsFullResponse.value = true;
    } else {
        // 保留 ```image 部分，移除 \n 后面的内容。
        // console.log('partial response', currentResponse.value.content);
        ++cmdIndex;
        currentResponse.value.content = currentResponse.value.content.substring(0, imageCodeBlock + 9) + currentResponse.value.content.substring(removalEnd);
    }

    // 执行命令
    if (cmd) execCommandInContext(cmd, 'code').then(({ success, error, stack, result }) => {
        if (!success) {
            currentFailures.value.push(stack || error);
            currentFailedIndex.value = cmdIndex;
        }
        else if (result === 'multiline_flag') {
            temporarilyMultipleLineExecution.value = true;
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
    context.value.push({
        role: 'user',
        content: inputMsg.value,
        timestamp: Date.now()
    });
    inputMsg.value = '';
    currentCode.value = '';
    currentResponse.value.id = crypto.randomUUID();
    currentFailures.value.length = 0;
    userScrolled.value = false;
    temporarilyMultipleLineExecution.value = false;

    await new Promise(nextTick);
    msgArea.value.scrollTop = msgArea.value.scrollHeight;
    toast.success("正在生成...");

    try {
        const { fetchEventSource } = await import('@microsoft/fetch-event-source');
        
        // 先清屏
        if (overlayMode.value === 0) await execCommandInContext('clear');
        let isFirstChunk = true;
        
        await fetchEventSource('https://api.deepseek.com/chat/completions', {
            openWhenHidden: true,
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
                        currentResponse.value.raw_content += data.choices[0].delta.content;
                    }
                    if (data.choices[0].delta.content) parse();
                    if (!userScrolled.value || auto_scroll.value) {
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
        currentCode.value = currentResponse.value.commands?.[currentResponse.value.nextCmd] || '';
        context.value.push({
            role: 'assistant',
            id: currentResponse.value.id,
            content: currentResponse.value.content,
            reasoning_content: currentResponse.value.reasoning_content,
            raw_content: currentResponse.value.raw_content,
            timestamp: Date.now(),
            commands: currentCode.value,
            cmd_list: currentResponse.value.commands,
        });
        currentResponse.value = { id: '', reasoning_content: '', content: '', raw_content: '', nextCmd: 0, commands: {} };
        await new Promise(nextTick);
        if (!userScrolled.value || auto_scroll.value) {
            msgArea.value.scrollTop = msgArea.value.scrollHeight;
        }
        // 尝试自动修复问题
        if (try_autorerun.value) nextTick(() => { rerunErrors(true) });
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
async function rerunErrors(silent = false) {
    if (currentFailedIndex.value === 0) {
        return false;
    }
    if (isGeneratingResponse.value) {
        toast.info("暂不可用，请等待生成完成");
        return;
    }
    const result = await drawCmd(context.value[context.value.length - 1], currentFailedIndex.value);
    if (result) {
        // 清除错误列表
        currentFailures.value.length = 0;
        currentFailedIndex.value = 0;
        if (silent !== true) toast.success('重新运行: 成功');
    } else {
        if (silent !== true) toast.warning('重新运行: 失败');
    }
    return result;
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
    if (await rerunErrors(true) === true) {
        toast.info('重新运行后错误自动得到了修复。');
        return;
    }
    const oldInputMsg = inputMsg.value;
    inputMsg.value = `你刚才提供的代码有错误，请修复这些错误:\n\n\`\`\`js\n${errorsStr}\n\`\`\`\n\n请修复错误后重新给出正确的、能够正常运行的代码。`;
    send();
    await new Promise(nextTick);
    inputMsg.value = oldInputMsg;
}
function render_lines(text) {
    return text.split('\n');
}
async function drawCmd(data, cmdIndex) {
    if (!data || !cmdIndex) return false;
    const cmd = (data.cmd_list || data.commands)[cmdIndex];
    if (!cmd) return
    currentCode.value = cmd;
    try {
        await execCommandInContext('clear');
        const { success, error, stack } = await execCommandInContext(cmd, 'code');
        if (!success) {
            currentFailures.value.push(stack || error);
            currentFailedIndex.value = 0;
        }
        else return true;
    } catch (e) {
        toast.error('命令执行失败: ' + e);
    }
}
function viewCmdSource(data, { detail: cmdIndex }) {
    const cmd = (data.cmd_list || data.commands)[cmdIndex];

    currentCode.value = cmd;
    codeShowingDialog.value.open = true;
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
textarea {
    resize: none;
}
.my-links > a+a {
    margin-left: 1em;
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
[data-role="user"] .message-header {
    color: #fff;
}

.role-error {
    border: 1px solid #dd0000;
}

.message-time {
    opacity: 0.7;
    margin-left: 0.5em;
}

.message-bubble[data-role="user"] {
    align-self: flex-end;
    background: #4a90e2;
    color: white;
}

.message-bubble[data-role="assistant"] {
    align-self: flex-start;
    background: #f0f0f0;
    color: #333;
}

.message-bubble:not([data-role="assistant"]):not([data-role="user"]):not(.role-error) {
    align-self: flex-start;
    border: 1px solid #dd0000;
}
.message-bubble:not([data-role="assistant"]):not([data-role="user"]):not(.role-error) .message-role::after {
    content: "(数据异常)";
    margin-left: 0.5em;
    color: red;
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
    line-height: 1.2;
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