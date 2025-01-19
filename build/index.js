function main() {
  document.getElementById("call-button").addEventListener("click", () => {
    const a = 10;
      Module._setContext(window.innerWidth, window.innerHeight, window.devicePixelRatio);
  });
}

window.onload = main;
