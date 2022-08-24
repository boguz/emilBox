var r=(n=>(n.Player="Player",n.Catalogue="Catalogue",n))(r||{}),o=class{constructor(t){this.defaultActiveId="Player";this.buttonActiveAttribute="active";this.contentActiveAttribute="active";this.container=t,this.tabsButtons=t.querySelectorAll(".tabs__button"),this.tabsContents=t.querySelectorAll(".tabs__content"),this.activeID=this.defaultActiveId,!(!this.tabsButtons||!this.tabsContents||this.tabsButtons.length!==this.tabsContents.length)&&(this.tabsButtons.forEach(n=>{n.addEventListener("click",this._handleButtonClick.bind(this))}),this._updateUI())}_updateUI(){this.tabsButtons.forEach(t=>{t.getAttribute("tab-id")===this.activeID?t.setAttribute(this.buttonActiveAttribute,""):t.removeAttribute(this.buttonActiveAttribute)}),this.tabsContents.forEach(t=>{t.getAttribute("tab-id")===this.activeID?t.setAttribute(this.contentActiveAttribute,""):t.removeAttribute(this.contentActiveAttribute)})}_handleButtonClick(t){let n=t.target.getAttribute("tab-id");this.activeID=r[n],this._updateUI()}};function c(){let e=document.querySelector(".tabs");!e||new o(e)}var a=class{constructor(t){this.input=t,this.input.addEventListener("input",this._updateUI.bind(this)),this._updateUI()}_updateUI(){let t=this._getPercentageOfValue(+this.input.min,+this.input.max,+this.input.value);this.input.style.background=`linear-gradient(to right, var(--color-primary-dark) 0%, var(--color-primary-dark) ${t}%, var(--color-primary) ${t}%, var(--color-primary) 100%)`}_getPercentageOfValue(t,n,d){return d*100/n}};function u(){let e=document.querySelector('input[type="range"]');!e||new a(e)}var p=`ws://${window.location.hostname}/ws`,i;function b(){i.send("getValues")}function s(){console.log("Trying to open a WebSocket connection\u2026"),i=new WebSocket(p),i.onopen=h,i.onclose=v,i.onmessage=m}function h(e){console.log("Connection opened"),b()}function v(e){console.log("Connection closed"),setTimeout(s,2e3)}function m(e){console.log(e.data);let t=JSON.parse(e.data),n=Object.keys(t)}function l(){s(),c(),u()}document.readyState==="complete"||document.readyState==="interactive"?setTimeout(l):document.addEventListener("DOMContentLoaded",l);
