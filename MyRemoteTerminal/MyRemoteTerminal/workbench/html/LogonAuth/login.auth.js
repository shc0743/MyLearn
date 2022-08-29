(document.getElementById('main') || {}).style.display = '';
(document.getElementById('iForgotMyPassword') || {}).onclick = function (e) {
    e.preventDefault()
    var m = window.iForgotMyPasswordModalDialog
    if (!m) {
        m = new ModalElement
        m.escapeClosable = true
        var i = document.createElement('iframe')
        var x = document.createElement('button')
        window.iForgotMyPasswordModalDialog = m
        i.src = '/api/auth/passwordrecovery/faq'
        x.innerHTML = 'x'
        x.onclick = function () {
            m.close()
        }
        i.style.position = x.style.position = 'fixed'
        i.style.left = i.style.top = 0
        i.style.width = i.style.height = '100%'
        i.style.border = '0'
        i.style.zIndex = x.style.zIndex = 81
        x.style.right = x.style.top = '10px'
        m.root.append(i)
        m.root.append(x)
    }
    m.showModal()
    return false
};
document.querySelectorAll('#main .input_box input').forEach(el => {
    el.onblur = function () {
        if (this.parentElement.querySelector('input:valid') == null) {
            this.parentElement.classList.add('invalid');
        } else {
            this.parentElement.classList.remove('invalid');
        }
    };
    el.onkeyup = function () {
        this.onblur();
    };
});

