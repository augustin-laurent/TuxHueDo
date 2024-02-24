function getVersion()
{
  let versionPromise = RequestUtils.get("/api/version");
  versionPromise.then((data) => {
    let versionSection = document.getElementById("versionSection");
    let text = `Version ${data.version}`;
    log("Huenicorn", text);
    versionSection.innerHTML = text;
  });
  versionPromise.catch((error) => {log(error);});
}

getVersion();
