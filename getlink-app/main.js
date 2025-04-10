let captcha;
let url = new URL(location.href);
const object = url.searchParams.get('object');
const bucket = url.searchParams.get('bucket');
const region = url.searchParams.get('region');
const type = url.searchParams.get('type') || 'link';
let locale = url.searchParams.get('locale') || 'en';

(async function() {
    let resp = await fetch('i18n/' + locale + '.json?v=' + v);
    if (!resp.ok) resp = await fetch('i18n/en.json?v=' + v);
    if (!resp.ok) throw resp.status;
    globalThis.i18n = await (resp.json());
    
    document.querySelectorAll("[data-i18n]").forEach(el => {
        const key = el.dataset.i18n;
        if (key in i18n) el.innerText = i18n[key]
    });
    document.title = i18n.title;
    
    if (!(object && bucket && region)) throw i18n.nolink;
    if (!isValidUrl(object)) throw i18n.link_not_supported;
    if (type !== "link" && type !== "content") throw i18n.unknown_type;
    
})()
.then(() => new Promise(async (resolve, reject) => {
    if (locale === 'zh') locale = 'cn'; // for compalibility to AliyunCaptcha
    const s = document.createElement("script");
    s.src = 'https://o.alicdn.com/captcha-frontend/aliyunCaptcha/AliyunCaptcha.js';
    s.onload = resolve;
    s.onerror = reject;
    aliyun_captcha_js.replaceWith(s);
}))
.then(() => {
    return main();
})
.catch((error) => {
    //alert(error);
    button.remove();
    if (typeof error === "string") tip.innerText = error;
    else tip.innerText = (window.i18n ? i18n.loadfail : '加载失败: ') + error;
    loading_placeholder.remove();
});

function isValidUrl(url) {
    try {
        // 条件（1）：检查 URL 是否以特定模式开头
        if (/^https:\/\/[a-zA-Z0-9_-]+\.[a-zA-Z0-9_-]+\.aliyuncs\.com\//.test(url)) {
            return true;
        }

        // 条件（2）：检查 URL 的根域名是否与当前页面的根域名一致
        const getRootDomain = (url) => new URL(url).hostname.match(/([a-zA-Z0-9-]+\.[a-zA-Z0-9-]+)$/)[0];
        return getRootDomain(url) === getRootDomain(window.location.href);
    } catch {
        // 如果遇到异常（如 URL 格式错误或正则匹配失败），返回 false
        return false;
    }
}

async function main() {
    // 弹出式，除region和prefix以外的参数
    window.initAliyunCaptcha({
        // 场景ID。根据步骤二新建验证场景后，您可以在验证码场景列表，获取该场景的场景ID
        SceneId: '1p7enydb',
        // 验证码模式。popup表示要集成的验证码模式为弹出式。无需修改
        mode: 'popup',
        // 触发验证码弹窗的元素。button表示单击登录按钮后，触发captchaVerifyCallback函数。您可以根据实际使用的元素修改element的值
        button: '#captcha_button',
        // 业务请求(带验证码校验)回调函数，无需修改
        captchaVerifyCallback: p => captchaVerifyCallback(p).catch(error=>({captchaResult:false,bizResult:false})),
        // 业务请求结果回调函数，无需修改
        onBizResultCallback: onBizResultCallback,
        // 绑定验证码实例函数，无需修改
        getInstance: getInstance,
        // 验证码语言类型，支持简体中文（cn）、繁体中文（tw）、英文（en）
        language: locale,
    });
    function getInstance(instance) {
        captcha = instance
        finish()
    }
            
    button.onclick = function () {
        captcha_button.click()
        //captcha_button.click()
    }
    
    let dlink = '', dvalid = 0;
    
    async function captchaVerifyCallback(captchaVerifyParam) {
        // console.log(captchaVerifyParam);
    
        // 1.向后端发起业务请求，获取验证码验证结果和业务结果
        const result = await (await fetch('https://alicaptchacheckv1.421013.xyz/', {
            method: 'POST',
            body: JSON.stringify({
                captcha: captchaVerifyParam,
                url: object,
                bucket: bucket,
                region: region,
            }),
        })).json();
    
        // 2.构造标准返回参数
        const verifyResult = {
            // 验证码验证是否通过，boolean类型，必选。
            captchaResult: result.captcha,
            // 业务验证是否通过，boolean类型，可选；若为无业务验证结果的场景，bizResult可以为空。
            bizResult: result.captcha,
        };
        if (result.captcha) {
            dlink = result.link;
            dvalid = result.expiration;
        }
        return verifyResult;
    }
    
    // 业务请求验证结果回调函数
    function onBizResultCallback(bizResult) {
        if (true !== bizResult) {
            return alert('业务验证不通过！');
        }
        button.remove();
        
        if (type === 'content') return queueMicrotask(async () => {
            tip.innerText = i18n.fetching_content;
            // 创建一个 AbortController 实例
            const controller = new AbortController();
            const signal = controller.signal;
            // 设置超时时间为 3 秒
            const timeout = 3000;
            // 使用 setTimeout 来设置超时
            const timeoutId = setTimeout(() => {
                controller.abort(); // 取消请求
            }, timeout);
            
            try {
                // 发起 fetch 请求
                const l = await fetch(dlink, { signal }).then(response => {
                    clearTimeout(timeoutId); // 清除超时计时器
                    if (!response.ok || (response.headers.get('content-length') > 8192)) {
                      throw i18n.link_content_invalid
                    }
                    return response.text();
                });
                tip.remove(); expiration_tip.remove();
                downloadui.hidden = false;
                downloadlink.href = downloadlink.innerText = l;
            } catch (error) {
                controller.abort(); // 取消请求
                if (error && error.name === "AbortError") tip.innerText = i18n.timeout;
                else tip.innerText = i18n.cant_get_link + error
            }
        })
        
        tip.remove();
        downloadui.hidden = false;
        downloadlink.href = downloadlink.innerText = dlink;
        
        // 改为基于时间戳的倒计时
        const startTime = Date.now();
        const expireTime = startTime + dvalid * 1000; // 转换为毫秒
        let timer;
        
        const updateTimer = () => {
            const remaining = Math.round((expireTime - Date.now()) / 1000);
            
            if (remaining <= 0) {
                downloadlink.href = 'javascript:;';
                downloadlink.innerText = i18n.expired || '链接已失效。点击重新加载';
                downloadlink.addEventListener('click', () => location.reload());
                clearInterval(timer);
                downloadtime.innerText = 0;
                return;
            }
            
            downloadtime.innerText = remaining;
        };

        updateTimer(); // 立即执行一次更新
        timer = setInterval(updateTimer, 1000);
        
    }
    
    function finish() {
        if (finish.ok) return
        button.classList.remove('is-disabled')
        button.disabled = button.hidden = false
        loading_placeholder.remove()
        finish.ok = true
    }
}

