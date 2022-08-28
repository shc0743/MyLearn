(document.getElementById('main') || {}).style.display = '';
(document.getElementById('iForgotMyPassword') || {}).onclick = function () {

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

