importScripts('oscillator.js');

let wasmModule = null;
let isRunning = false;
let calibrationData = null;

// 初始化WASM
async function initializeWASM() {
    if (!wasmModule) {
        wasmModule = await oscillator();
        // 进行更精确的校准
        calibrationData = await preciseCalibration();
    }
    return calibrationData;
}

// 精确校准
async function preciseCalibration() {
    const testIterations = 32768;
    const samples = 8;
    
    if (!wasmModule || !wasmModule._intensive_calculation) {
        throw new Error('WASM模块未正确加载');
    }
    
    const measureExecution = () => {
        const start = performance.now(); // 使用 performance.now() 更精确
        wasmModule._intensive_calculation(testIterations);
        return performance.now() - start;
    };
    
    const measurements = [];
    for (let i = 0; i < samples; i++) {
        const delta = (measureExecution());
        console.log(`[Worker] 第`, i, '次校准，结果:', delta)
        measurements.push(delta);
    }
    
    const averageTime = measurements.reduce((sum, time) => sum + time, 0) / measurements.length;
    const iterationsPerMs = testIterations / averageTime;
    const singleCost = averageTime;
    
    // 计算标准差
    const stdDev = Math.sqrt(
        measurements.reduce((sum, time) => sum + Math.pow(time - averageTime, 2), 0) / measurements.length
    );
    
    console.log(`[Worker] 校准完成: ${iterationsPerMs.toFixed(2)} 迭代/ms, 平均时间: ${averageTime.toFixed(2)}ms, 标准差: ${stdDev.toFixed(2)}ms`);
    
    return {
        iterationsPerMs,
        singleCost,
        averageTime,
        stdDev,
        measurements,
        confidence: stdDev / averageTime // 变异系数，越小越稳定
    };
}

// 高精度短时计时
function preciseTimer(durationMs, callback) {
    if (!wasmModule || !calibrationData) {
        throw new Error('WASM未初始化');
    }

    isRunning = true;
    const startTime = performance.now(); // 使用更高精度的performance.now()
    
    // 计算需要执行的迭代次数（考虑校准数据）
    const targetIterations = Math.floor(durationMs * calibrationData.iterationsPerMs);
    console.log(`[Worker] 目标: ${durationMs}ms, 需要迭代: ${targetIterations}次`);
    
    // 执行计算
    wasmModule._intensive_calculation(targetIterations);
    
    const endTime = performance.now();
    const actualDuration = endTime - startTime;
    isRunning = false;
    
    console.log(`[Worker] 实际耗时: ${actualDuration.toFixed(2)}ms`);
    
    callback({
        scheduled: durationMs,
        actual: actualDuration,
        error: Math.abs(actualDuration - durationMs)
    });
}

// 处理主线程消息
self.addEventListener('message', async function(e) {
    const { type, duration } = e.data;
    
    switch (type) {
        case 'init':
            try {
                await initializeWASM();
                self.postMessage({ type: 'init_success', data: calibrationData });
            } catch (error) {
                self.postMessage({ type: 'init_error', error: error.message });
            }
            break;
            
        case 'start_precise':
            if (isRunning) {
                self.postMessage({ type: 'error', error: '计时器正在运行' });
                return;
            }
            preciseTimer(duration, (result) => {
                self.postMessage({ type: 'timer_end', result });
            });
            break;
            
        case 'stop':
            isRunning = false;
            self.postMessage({ type: 'stopped' });
            break;
    }
});