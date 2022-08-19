(function () {
    var sidebar_split = document.getElementById('sidebar_split');
    var sidebar_username = document.getElementById('sidebar_username');
    var sdb_l = document.querySelector('.sidebar-left');
    var sdb_r = document.querySelector('.sidebar-right');
    var disconnectbtn = document.getElementById('disconnectbtn');

    var __sdlf_w = 0;
    function resizeF() {
        if (document.body.clientWidth < 800) {
            if (!__sdlf_w) __sdlf_w = sdb_l.offsetWidth;
            sdb_l.style.width = '0px';
            sdb_l.hidden = true;
        }
        else if (__sdlf_w) {
            sdb_l.style.width = __sdlf_w + 'px';
            sdb_l.hidden = false;
            __sdlf_w = 0;
        }
        sdb_r.style.width = document.body.clientWidth - sdb_l.offsetWidth + 'px';
        sidebar_split.style.left = sdb_l.offsetWidth + 1 + 'px';
    }
    if (typeof(Window.this) !== 'undefined') {
        Window.this.on('size', resizeF)
        Window.this.on("statechange", resizeF)
    } else window.addEventListener('resize', resizeF);
    resizeF();

    sidebar_split.onmousedown = function (ev) {
        ev.preventDefault();

        function mm(ev) {
            document.querySelector('.sidebar-left').style.width = ev.x + 'px';
            resizeF();
        }
        function mu() {
            document.removeEventListener('mousemove', mm);
            document.removeEventListener('mouseup', mu);

        }

        document.addEventListener('mousemove', mm);
        document.addEventListener('mouseup', mu)
    }
    sidebar_split.onkeydown = function (ev) {
        if (ev.key == 'ArrowLeft') {
            document.querySelector('.sidebar-left').style.width =
                document.querySelector('.sidebar-left').clientWidth - 1
                + 'px';
            resizeF();
        }
        else if (ev.key == 'ArrowRight') {
            document.querySelector('.sidebar-left').style.width =
                document.querySelector('.sidebar-left').clientWidth + 1
                + 'px';
            resizeF();
        }
    }

    sidebar_username.onclick = function () {

    }

    var panels_list = document.querySelectorAll('#panel_choose .item');
    panels_list.forEach(element => {
        element.onkeydown = function (ev) {
            if (ev.keyCode == 13) return this.click();
        }
        element.onclick = function () {
            location.hash = '#' + this.getAttribute('data-panel');
            F_hashchange();
        }
    });

    function F_hashchange() {
        var hash = location.hash || '';
        if (hash == '') {
            location.hash = '#general';
            return;
        }
        if (hash.indexOf('#') != 0) return;

        hash = hash.substring(1);

        for (let i of panels_list) {
            i.removeAttribute('data-checked');
        }
        let pss = document.querySelector(`#panel_choose .item[data-panel="${hash}"]`);
        if (pss) {
            pss.setAttribute('data-checked', true);
        }

        // document.getElementById('sidebar_function_select_hider').innerHTML =
        //     `.sidebar .content .page[data-panel="${hash}"] { display: block; }`;
        
        let li2 = document.querySelectorAll(`.sidebar .content .page[data-panel]`);
        for (let j of li2) {
            j.style.display = (j.getAttribute('data-panel') == hash ? 'block' : 'none');
        }

    }
    (Window.this || window).addEventListener('hashchange', F_hashchange);
    F_hashchange();

    document.getElementById('b_Exit').onclick = function () {
        // var bg = document.createElement('div');
        // bg.setAttribute('style', `background:rgba(0,0,0,0.5);
        // position:fixed;z-index:99998;left:0;top:0;width:100%;height:100%;`);
        // document.body.append(bg);
        // var dlg = document.createElement('div');
        // dlg.innerHTML = `
        // <h2>Are you sure you want to exit?</h2>
        // <div>
        // <button style="display:block;font-size:larger;width:1*"
        //  class=yes>Yes</button>
        // <button style="display:block;font-size:larger;width:1*;margin-top:10px"
        //  class=no>No</button>
        // </div>
        // `;
        // dlg.querySelector('.yes').onclick = function () {
        //     (Window.this || window).close(true);
        // }
        // dlg.querySelector('.no').onclick = function () {
        //     bg.remove(); dlg.remove();
        // }
        // dlg.setAttribute('style', `
        // position: fixed;
        // left: 50%; top: 50%;
        // transform: translate(-50%, -50%);
        // z-index: 99999;
        // background: #ffffff;
        // text-align: center;
        // padding: 10px;
        // width: 50%;
        // border: 1px solid #ccc;
        // `);
        // document.body.append(dlg);
        //// dlg.showModal();

        if (Window.this.MainUI_wnd.Confirm("Are you sure you want to exit?",
            "Exit Application - My Remote Tool", 0x30 | 0x100, 0x20)) {
            // Window.this.MainUI_wnd.setClosable(true);
            // Window.this.close(true);
            Window.this.MainUI_wnd.ApplicationQuit();
        }
        
    }

    document.getElementById('about__reloadpage').onclick = function () {
        Window.this.MainUI_wnd.nativeReloadPage();
    }

    document.querySelectorAll('a[href]').forEach(el => {
        if (el.getAttribute('href').indexOf('http') == 0) {
            el.onclick = function () {
                Window.this.MainUI_wnd.openWebPage(this.getAttribute('href'));
            }
        }
    });

    document.getElementById('connect_ip').on('change', function () {
        if (this.value.split(':').length == 2) {
            let arr = this.value.split(':');
            this.value = arr[0];
            document.getElementById('connect_port').value = arr[1];
        }
    })

    function updateConnectionStatus() {
        if (window.debug_pause) return;
        var conn_info = Window.this.MainUI_wnd.getConnectionInfo();
        if (conn_info.connectstate == 1) {
            document.querySelector('.network-status-tip-statbox').style.background = 'lightgreen';
            document.querySelector('.network-status-tip-text').innerHTML = 'Connected';
            document.getElementById('connection_form').style.display = 'none';
            document.querySelector('div.connected').style.display = 'block';
            document.querySelector('.tryingtoconnect').style.display = 'none';
        }
        else if (conn_info.connectstate == 2) {
            document.querySelector('.network-status-tip-statbox').style.background = '#dd0';
            document.querySelector('.network-status-tip-text').innerHTML = 'Connecting';
            document.getElementById('connection_form').style.display = 'none';
            document.querySelector('div.connected').style.display = 'none';
            document.querySelector('.tryingtoconnect').style.display = 'block';
        }
        else if (conn_info.connectstate == 0) {
            document.querySelector('.network-status-tip-statbox').style.background = 'red';
            document.querySelector('.network-status-tip-text').innerHTML = 'Unconnected';
            document.getElementById('connection_form').style.display = 'block';
            document.querySelector('div.connected').style.display = 'none';
            document.querySelector('.tryingtoconnect').style.display = 'none';
        }
    }
    setInterval(updateConnectionStatus, 1000);
    updateConnectionStatus();

    document.querySelector('.about_archshow').innerHTML =
        Window.this.MainUI_wnd.getArchString();

    document.getElementById('connection_form').onsubmit = function () {
        if (Window.this.MainUI_wnd.tryConnect({
            ip: document.getElementById('connect_ip').value,
            port: String(document.getElementById('connect_port').value),
        })) {
            this.querySelectorAll('input, button[type="submit"]').forEach(el => el.disabled = true);
            this.parentElement.querySelector('.tryingtoconnect').hidden = false;

        } else {
            Window.this.MainUI_wnd.msgbox("Cannot connect (internal error)", "Error", 0x10);
        }
    }

    document.getElementById('open_license').onclick = function () {
        Window.this.MainUI_wnd.openWebPageSciter(
            'https://www.gnu.org/licenses/lgpl-3.0-standalone.html');
        return false;
    }

    disconnectbtn.onclick = function () {
        Window.this.MainUI_wnd.disconnectSocket();
    }

}())