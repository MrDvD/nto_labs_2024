async function init(url) {
    let response = await fetch(url, {
        method: 'GET'
    });
    if (response.status == 200) {
        let cm = parseFloat(await response.text());
        document.getElementsByClassName("distance")[0].innerHTML = `${cm} cm`;
        let perc = Math.min(75, Math.max(15, 1.5 * cm));
        document.getElementsByClassName("line")[0].style.width = `${perc}%`;
        if (cm < 20) {
            document.getElementsByClassName("diff")[0].innerHTML = `By<br>${20 - cm} cm`;
            document.getElementsByClassName("warning")[0].style.visibility = "visible";
        } else {
            document.getElementsByClassName("warning")[0].style.visibility = "hidden";
        }
    } else {
        await init(url);
    }
}

async function update(url) {
    let response = await fetch(url, {
        method: 'GET'
    });
    if (response.status == 200) {
        let cm = parseFloat(await response.text());
        document.getElementsByClassName("distance")[0].innerHTML = `${cm} cm`;
        let perc = Math.min(75, Math.max(15, 1.5 * cm));
        document.getElementsByClassName("line")[0].style.width = `${perc}%`;
        if (cm < 20) {
            document.getElementsByClassName("diff")[0].innerHTML = `By<br>${20 - cm} cm`;
            document.getElementsByClassName("warning")[0].style.visibility = "visible";
        } else {
            document.getElementsByClassName("warning")[0].style.visibility = "hidden";
        }
    }
    await update(url);
}

window.onload = function() {
    init('http://192.168.45.242:7000/init');
    update('http://192.168.45.242:7000/update')
}