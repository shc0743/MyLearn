let captcha;
let url = new URL(location.href);
const object = url.searchParams.get('object');
const bucket = url.searchParams.get('bucket');
const region = url.searchParams.get('region');
if (!(object && bucket && region)) {
    button.remove();
    tip.innerText = '没有指定链接。';
}
// 弹出式，除region和prefix以外的参数
window.initAliyunCaptcha({
    // 场景ID。根据步骤二新建验证场景后，您可以在验证码场景列表，获取该场景的场景ID
    SceneId: '1p7enydb',
    // 验证码模式。popup表示要集成的验证码模式为弹出式。无需修改
    mode: 'popup',
    // 触发验证码弹窗的元素。button表示单击登录按钮后，触发captchaVerifyCallback函数。您可以根据实际使用的元素修改element的值
    button: '#button',
    // 业务请求(带验证码校验)回调函数，无需修改
    captchaVerifyCallback: p => captchaVerifyCallback(p).catch(error=>({captchaResult:false,bizResult:false})),
    // 业务请求结果回调函数，无需修改
    onBizResultCallback: onBizResultCallback,
    // 绑定验证码实例函数，无需修改
    getInstance: getInstance,
    // 验证码语言类型，支持简体中文（cn）、繁体中文（tw）、英文（en）
    language: 'cn',
});
function getInstance(instance) {
    captcha = instance
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
// 业务请求验证结果回调函数
function onBizResultCallback(bizResult) {
    if (bizResult === true) {
        button.remove();
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
                downloadlink.innerText = '链接已失效。点击重新加载';
                downloadlink.addEventListener('click', () => location.reload());
                clearInterval(timer);
                downloadtime.innerText = 0;
                return;
            }
            
            downloadtime.innerText = remaining;
        };

        updateTimer(); // 立即执行一次更新
        timer = setInterval(updateTimer, 1000);
    } else {
        alert('业务验证不通过！');
    }
}


button.classList.remove('is-disabled')
button.disabled = button.hidden = false
loading_placeholder.remove()
