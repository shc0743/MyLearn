document.getElementById('btn_close').on('click', () => Window.this.close());

const diskList = document.getElementById('diskList');

async function Load() {
    const dev = document.getElementById('deviceClasses');
    const dsk = document.getElementById('diskList');
    dev.innerHTML = '';
    for (const i of Window.this.UI_Main.getDevices()) {
        const el = document.createElement('option');
        el.innerText = i.split('|')[1] || i;
        dev.append(el);
    }


    const disks = Window.this.UI_Main.getDiskList();
    dsk.innerHTML = '';
    if (!disks) {
        dsk.patch(<option><b>暂无USB存储设备插入。</b></option>)
    } else {
        dsk.patch(disks.map(i => {
            const iar = i.split('|');
            const it = iar[1] || i;
            const iv = iar[0] || i;
            return <option value={iv}>
                {it}
            </option>
        }));
        if (disks.length) {
            dsk.value = disks[0].split('|')[0] || disks[0];
            document.$('#recentWrapper').removeAttribute('hidden');
            document.$('#openDisk').removeAttribute('hidden');
            document.$('#openDisk').disabled = false;
        }
    }

    setTimeout(loadVolFileChanges, 100);
    
}

function loadVolFileChanges() {
    const r = Window.this.UI_Main.findVolumeFileList(diskList.value);
    const rct = document.getElementById('recent');
    rct.innerHTML = '';
    if (!r) {
        rct.patch(<span>如果您最近使用了一些文件，那么它们会显示在这里。</span>);
        return;
    }

    rct.patch(r.map(i => {
        return <div class="recent-file-item" data-file={i.path}>
            <div class="left">
                <div class="file-name">{i.name}</div>
                <div class="file-info">{i.path}</div>
                <div class="file-info">修改日期：{i.time}</div>
            </div>
            <button class="right">打开</button>
        </div>
    }));
    
    document.querySelector('#loading')?.remove();
}
document.getElementById('diskList').on('input', loadVolFileChanges);
document.getElementById('recent').on('click', function (ev) {
    const t = ev.target; if (!t) return;
    Window.this.UI_Main.openFile(t.parentElement.getAttribute('data-file'));
})
document.getElementById('reloadList').on('click', () => Load());

document.getElementById('openDisk').on('click', () => Window.this.UI_Main.openDisk(diskList.value));



document.$('#deviceClassesWrapper').on('contextmenu', () => Window.this.modal(<alert caption="设备">
    <content>{Window.this.UI_Main.getDevices().join('\n')}</content>
</alert>));

setTimeout(() => {
    document.querySelector('#loading')?.remove();
}, 5000);
    
