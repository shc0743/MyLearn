let puppeteer;
import { exec } from 'child_process';
import { promisify } from 'util';
import { readFile } from 'fs/promises';

function autoScroll(page) {
    return page.evaluate(() => {
        return new Promise((resolve, reject) => {
            var totalHeight = 0;
            var distance = 100;
            var timer = setInterval(() => {
                var scrollHeight = document.body.scrollHeight;
                window.scrollBy(0, distance);
                totalHeight += distance;
                if (totalHeight >= scrollHeight) {
                    clearInterval(timer);
                    resolve();
                }
            }, 100);
        })
    });
}

async function sha256(input) {
    return Array.from(new Uint8Array(await crypto.subtle.digest('SHA-256', (new TextEncoder()).encode(input)))).map(b => b.toString(16).padStart(2, '0')).join('');
}


export async function handler(event, context) {
    event = JSON.parse(event);
    const url = event.requestContext.http.path.substring(1);
    const method = event.requestContext.http.method;
    
    if (method === 'OPTIONS') {
        return {
            statusCode: 200,
            headers: {
                'Access-Control-Allow-Origin': '*',
                'Access-Control-Allow-Methods': 'GET, POST, OPTIONS',
                'Access-Control-Allow-Headers': 'Content-Type'
            },
            body: ''
        };
    }

    if (process.env.password) {
        if (process.env.password !== (await sha256(event.headers.Authorization + process.env.salt))) return {
            statusCode: 401, headers: { 'WWW-Authenticate': 'Basic' }
        }
    }
    // password should be SHA256('Basic <Base64-encoded username:password>' + salt)
    
    if (method === 'GET' && url === 'favicon.ico') {
        try {
            const faviconPath = './favicon.ico';
            const faviconContent = await readFile(faviconPath);
            return {
                statusCode: 200,
                headers: { 'Content-Type': 'image/x-icon' },
                body: faviconContent.toString('base64'),
                isBase64Encoded: true
            };
        } catch (error) {
            return {
                statusCode: 404,
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify({ error: 'Favicon not found' })
            };
        }
    }
    
    const params = {
        w: 1920, h: 1080
    };
    if (method === 'POST') try {
        if (event.isBase64Encoded) event.body = Buffer.from(event.body, 'base64').toString('utf-8');
        const { width, height } = JSON.parse(event.body);
        params.w = +width;
        params.h = +height;
        if (isNaN(params.w) || isNaN(params.h)) throw 0;
        if (params.w < 1 || params.w > 9999) throw 1;
        if (params.h < 1 || params.h > 9999) throw 2;
    } catch {
        return {
            statusCode: 400,
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({ error: 'Invalid request body' })
        };
    }

    try { new URL(url) } catch {
        return {
            statusCode: 400,
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({ error: 'Invalid URL', url })
        };
    }

    try {
        await ((promisify(exec))("ln -s /opt/node_modules node_modules"));
    } catch {}

    try {
        puppeteer = await import('puppeteer');
        
        const browser = await puppeteer.launch({
            headless: true,
            args: [
                '--disable-gpu',
                '--disable-dev-shm-usage',
                '--disable-setuid-sandbox',
                '--no-first-run',
                '--no-zygote',
                '--no-sandbox',
                '--single-process'
            ],
            executablePath: '/opt/home/codespace/.cache/puppeteer/chrome/linux-140.0.7339.82/chrome-linux64/chrome'
        });
        const page = await browser.newPage();
        await page.emulateTimezone('Asia/Shanghai');
        await page.goto(url, {
            'waitUntil': 'networkidle2'
        });
        await page.setViewport({
            width: params.w,
            height: params.h
        });
        await autoScroll(page)
        let path = '/tmp/image.png';
        await page.screenshot({ path: path, fullPage: true, type: 'png' });
        await browser.close();
        const fileContent = await readFile(path);
        return {
            statusCode: 200,
            headers: { 'Content-Type': 'image/png' },
            body: fileContent.toString('base64'),
            isBase64Encoded: true
        };
    }
    catch (error) {
        return {
            statusCode: 500,
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({ error: 'Unable to screenshot', str: String(error), stack: error?.stack })
        };
    }
}

/**
 * 验证URL格式
 * @param {string} string - 需要验证的URL
 * @returns {boolean} 是否有效URL
 */
function isValidUrl(string) {
    try {
        new URL(string);
        return true;
    } catch {
        return false;
    }
}
