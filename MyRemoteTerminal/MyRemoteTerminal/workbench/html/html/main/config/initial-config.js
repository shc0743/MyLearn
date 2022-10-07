(function () {

    var steps = [];
    var current_step_index = 0;
    
    fetch('get_step', {
        method: 'post',
        body: (window.getUrlValue && getUrlValue('session')) || ''
    })
    .then(function (v) {
        if (v.status >= 200 && v.status <= 399)
            return v.text()
        else
            throw new TypeError(`Failed to fetch (result: ${v.status} (${v.statusText}))`)
    })
    .then(function (t) {
        steps = (function () {
            let temp = t.split('|'), temp2 = [];
            temp.forEach(el => {
                let tmp = el.split('=');
                if (tmp.length !== 2) return;
                temp2.push({ key: tmp[0], value: tmp[1] });
            });
            return temp2;
        })();

        let ls = document.querySelector('#myForm .steps');
        if (ls) {
            ls.innerHTML = '';
            steps.forEach(el => {
                let p = document.createElement('p');
                p.classList.add('item');
                p.innerText = el.value;
                ls.append(p);
            })
            let n1;
            if ((n1 = ls.querySelector('p:nth-child(1)'))) {
                n1.classList.add('full');
                n1.classList.add('active');
            }
        }

        getCurrentStep();
    
    })
    .catch(function (e) {
        (document.querySelector('#myForm .content.ajax my-text') || {}).innerHTML =
            `Failed to load content because <my-error style="user-select:all" tabindex=0>Placeholder</my-error>.<br>
            Please try to <a href="javascript:location.reload()">Reload</a> this page.<br>
            If the error continues to occur, please contact your system administrator.`;
        (document.querySelector('#myForm .content.ajax my-text my-error') || {}).innerText = String(e);
    });

    async function getStepContent(id) {
        try {
            let fres = await fetch('get_step_content', {
                method: 'post',
                body: id
            });
            if (!fres.ok) throw new Error('HTTP Error ' + fres.status + ' ' + fres.statusText);
            let cont = await fres.text();
            return cont;
        }
        catch (error) {
            throw error;
        }
    }

    function getCurrentStep() {
        document.querySelectorAll('#myForm .btn.control button').forEach(el => el.disabled = true);
        let error_handler = e => {
            console.error(e);
            document.querySelector('#myForm .content.ajax').innerHTML = `
            <h1>Fatal Error</h1>
            <div style="font-size:larger;color:red">
                <span>Failed to get step </span>
                <span s title="Copy" style="user-select:all" tabindex=0></span>
                <span> content.</span>
            </div>
            <div>
                <div>Reason:</div>
                <div title="Copy" style="user-select:all" tabindex=0 r></div>
            </div>
            <div>Try to <a href="javascript:location.reload()">Reload the page</a>.</div>
            `;
            document.querySelector('#myForm .content.ajax [r]').innerText = e;
            try { document.querySelector('#myForm .content.ajax [s]').innerText = `${steps[current_step_index].value} (${steps[current_step_index].key})` } catch (_) { };

        };

        try {
            getStepContent(steps[current_step_index].key)
            .then(v => {
                document.querySelector('#myForm .content.ajax').innerHTML = '';
                document.querySelector('#myForm .content.ajax').append(document.createElement('iframe'));
                // document.querySelector('#myForm .content.ajax iframe').src = v;
                fetch(v, { method: 'post', body: getUrlValue('session') })
                .then(v => { return (v.text()) })
                .then(text => {
                    document.querySelectorAll('#myForm .btn.control button').forEach(el => el.disabled = false);
                    if (current_step_index == 0)
                        document.querySelectorAll('#myForm .btn.control button[data-role=prev]').forEach(el => el.disabled = true);
                    let items = document.querySelectorAll('#myForm .steps .item');
                    items.forEach(e => e.classList.remove('active'));
                    items[current_step_index].classList.add('full', 'active');
                    for (let i = current_step_index + 1; i < items.length; ++i)
                        items[i].classList.remove('full');
                    document.querySelector('#myForm .content.ajax iframe').contentWindow.document.write(text);
                })
                .catch(error => {
                    error_handler(error);
                });
            })
            .catch(error_handler)
        }
        catch (error) { error_handler(error) }
    }

    function min() {
        let _ = null;
        for (let __ of arguments) {
            if (!_) _ = __;
            if (__ < _) _ = __;
        }
        return _;
    }
    function max() {
        let _ = null;
        for (let __ of arguments) {
            if (!_) _ = __;
            if (__ > _) _ = __;
        }
        return _;
    }
    
    document.querySelector('#myForm .btn.control button[data-role="prev"]').onclick = function () {
        current_step_index = max(current_step_index - 1, 0);
        getCurrentStep();
    }
    document.querySelector('#myForm .btn.control button[data-role="next"]').onclick = function () {
        current_step_index++;
        getCurrentStep();
    }
    document.querySelector('#myForm .btn.control button[data-role="cancel"]').onclick = function () {
        let el = new ModalElement();
        let c = 'c' + Math.random().toString().replaceAll('.','');

        el.element.classList.add(c);

        el.element.innerHTML = `
        <style>.${c} *{margin:5px;padding:5px;}</style>
        <div style="display:flex;flex-direction:column">
        <h1>Are you sure you want to cancel?</h1>
        <button y>Yes</button>
        <button n>No</button>
        </div>
        `;
        el.element.querySelector('[y]').onclick = function () {
            window.close()
            let w = window.open('', '_self')
            w.close()
            location = '/'
        };
        el.element.querySelector('[n]').onclick = function () {
            el.close();
            el.root.remove();
        };

        el.showModal();
    }

}());
