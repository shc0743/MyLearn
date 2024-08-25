
import * as sys from "@sys";
import { launch, home, PLATFORM } from "@env";

async function launchInspector() {
    const SUFFIX = { Windows: ".exe", OSX: ".app" };
    const PREFIX = { Windows: "\\", OSX: "/../../../" };

    let inspectorPath = home((PREFIX[PLATFORM] || "") + "inspector" + (SUFFIX[PLATFORM] || ""));

    try {
        await sys.fs.stat(inspectorPath);
        launch(inspectorPath);
    } catch (e) {
        Window.modal(<alert>Cannot find {inspectorPath}</alert>);
    }
}

launchInspector()