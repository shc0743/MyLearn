import { app, BrowserWindow, dialog } from 'electron';
import minimist from 'minimist';
const args = minimist(process.argv);

console.log("app: startup");

// 解析命令行参数
const url = args.url;
console.debug("parameter: url=", url);
if (!url) {
    app.whenReady().then(async () => {
        console.log("app: will quit");
        // 显示消息框
        const { response } = await dialog.showMessageBox({
            type: 'info',
            title: 'Usage',
            message: 'application --url=<url> [--click=<DomSelector>] [--execute=<JS>] [--all] [--delay=<ms>] [--topmost] [--fullscreen] [--time=<Seconds>] [--prevent=(close[,]|exitfullscreen)]',
            buttons: ['Oh, I got it!', 'Sorry, I couldn\'t understand']
        });
        if (response === 0) {
            // 退出应用
            app.quit();
        }
        if (response === 1) {
            const { response } = await dialog.showMessageBox({
                type: 'question',
                title: 'Really?',
                message: 'Really?',
                buttons: ['Yes, of course!', 'No, I have cheated you']
            });
            if (response === 0) {
                await dialog.showMessageBox({
                    type: 'warning',
                    title: 'Sorry!',
                    message: 'Please see API document for more information.',
                    buttons: ['Thank you', "You're welcome"]
                });
            }
            if (response === 1) {
                await dialog.showMessageBox({
                    type: 'error',
                    title: 'Angry!',
                    message: 'I am angry!',
                    buttons: ['Thank you', "You're welcome"]
                });
            }
            app.quit();
        }
    });
}

const click = args.click;
console.debug("parameter: click=", click);
const execute = args.execute;
console.debug("parameter: execute=", execute);
const delay = args.delay || 0;
console.debug("parameter: delay=", delay);

const time = args.time;
console.debug("parameter: time=", time);
const prevent = String(args.prevent || '').split(",");
console.debug("parameter: prevent=", prevent);

/**
 * execute JavaScript code in the window
 * @param {BrowserWindow} win window to be executed
 * @param {string} js JavaScript code to be executed
 */
const runjs = (win, js) => {
    console.log("app: run script =", js);
    return win.webContents.executeJavaScript(js, true)
       .then(result => {
            console.log("app: script result =", result);
        }).catch(error => {
            console.warn("app: script error =", error);
        });
}

const createWindow = () => {
    console.log("win: create window");

    let width = (+args.width);
    if (Number.isNaN(width)) width = 800;
    console.debug("parameter: width=", width);
    let height = (+args.height);
    if (Number.isNaN(height)) height = 600;
    console.debug("parameter: height=", height);
    
    const win = new BrowserWindow({
        width, height,
        autoHideMenuBar: true,
        alwaysOnTop: !!args.topmost,
        webPreferences: {
            autoplayPolicy: 'no-user-gesture-required',
        },
    });

    win.loadURL(url);
    if (!!args.fullscreen) win.fullScreen = true;

    if (prevent.includes("close")) win.addListener('close', (event) => {
        event.preventDefault();
    });
    if (prevent.includes("exitfullscreen")) win.addListener('leave-full-screen', () => {
        console.log("win: leave full screen");
        setTimeout(() => win.fullScreen = true);
    });

    let firstInstance = true;
    win.webContents.on('did-finish-load', () => {
        console.log("win: did-finish-load");
        if (!firstInstance && (!args.all)) return;
        firstInstance = false;
        if (click) setTimeout(() => runjs(win, `document.querySelector(\`${click}\`).click()`), delay);
        if (execute) setTimeout(() => runjs(win, execute), delay);
    });
    win.webContents.setWindowOpenHandler((details) => {
        console.log("win: setWindowOpenHandler");
        
        setTimeout(() => win.loadURL(details.url));

        return ({ "action": "deny" });
    });

    return win;
}

console.log("app: waiting for ready");
app.whenReady().then(() => {
    console.log("app: ready");
    if (!url) return;
    const win = createWindow()
    app.on('window-all-closed', () => {
        console.log("app: window-all-closed");
        if (process.platform !== 'darwin') {
            console.log("app: quit");
            app.quit()
        }
    })

    if (time && !Number.isNaN(time) && time > 0) {
        const currentTime = new Date();
        setInterval(() => {
            const elapsedTime = (new Date() - currentTime); // 计算经过的秒数
            if (elapsedTime >= (time * 1000)) {
                console.log("app: time triggered");
                win.destroy();
            }
        }, 2000);
    }
})
