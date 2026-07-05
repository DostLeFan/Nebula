const path = require("path");

const platform = process.platform;
const arch = process.arch;

module.exports = require(path.join(__dirname, "prebuilds", `${platform}-${arch}`, "nebula.node"));