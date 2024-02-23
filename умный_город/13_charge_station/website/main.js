let server_ip = '{{ context.ip }}', web_port = {{ context.web_port }};

window.onload = function() {
    document.getElementsByClassName('button')[0].addEventListener('click', function() {
        window.location.href = `http://${server_ip}:${web_port}/charge`;
    });
}