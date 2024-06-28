let range = document.querySelector(".timingRangeInput");
let number = document.querySelector('.timingInput');

if(range != null){
    range.addEventListener("input", (e) => {
        number.value = e.target.value;
    });    
}

if(number != null){
    number.addEventListener("change", (e) => {
        range.value = e.target.value;
    });
}
