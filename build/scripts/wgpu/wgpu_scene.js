// include: shell.js
// The Module object: Our interface to the outside world. We import
// and export values on it. There are various ways Module can be used:
// 1. Not defined. We create it here
// 2. A function parameter, function(moduleArg) => Promise<Module>
// 3. pre-run appended it, var Module = {}; ..generated code..
// 4. External script tag defines var Module.
// We need to check if Module already exists (e.g. case 3 above).
// Substitution will be replaced with actual code on later stage of the build,
// this way Closure Compiler will not mangle it (e.g. case 4. above).
// Note that if you want to run closure, and also to use Module
// after the generated code, you will need to define   var Module = {};
// before the code. Then that object will be used in the code, and you
// can continue to use Module afterwards as well.
var Module = typeof Module != 'undefined' ? Module : {};

// Determine the runtime environment we are in. You can customize this by
// setting the ENVIRONMENT setting at compile time (see settings.js).

// Attempt to auto-detect the environment
var ENVIRONMENT_IS_WEB = typeof window == 'object';
var ENVIRONMENT_IS_WORKER = typeof WorkerGlobalScope != 'undefined';
// N.b. Electron.js environment is simultaneously a NODE-environment, but
// also a web environment.
var ENVIRONMENT_IS_NODE = typeof process == 'object' && typeof process.versions == 'object' && typeof process.versions.node == 'string' && process.type != 'renderer';
var ENVIRONMENT_IS_SHELL = !ENVIRONMENT_IS_WEB && !ENVIRONMENT_IS_NODE && !ENVIRONMENT_IS_WORKER;

if (ENVIRONMENT_IS_NODE) {

}

// --pre-jses are emitted after the Module integration code, so that they can
// refer to Module (if they choose; they can also define Module)
// include: /var/folders/hx/g36pxlqs1dj0kvmzszq_j3k00000gq/T/tmpsatpkwed.js

  Module['expectedDataFileDownloads'] ??= 0;
  Module['expectedDataFileDownloads']++;
  (() => {
    // Do not attempt to redownload the virtual filesystem data when in a pthread or a Wasm Worker context.
    var isPthread = typeof ENVIRONMENT_IS_PTHREAD != 'undefined' && ENVIRONMENT_IS_PTHREAD;
    var isWasmWorker = typeof ENVIRONMENT_IS_WASM_WORKER != 'undefined' && ENVIRONMENT_IS_WASM_WORKER;
    if (isPthread || isWasmWorker) return;
    var isNode = typeof process === 'object' && typeof process.versions === 'object' && typeof process.versions.node === 'string';
    function loadPackage(metadata) {

      var PACKAGE_PATH = '';
      if (typeof window === 'object') {
        PACKAGE_PATH = window['encodeURIComponent'](window.location.pathname.substring(0, window.location.pathname.lastIndexOf('/')) + '/');
      } else if (typeof process === 'undefined' && typeof location !== 'undefined') {
        // web worker
        PACKAGE_PATH = encodeURIComponent(location.pathname.substring(0, location.pathname.lastIndexOf('/')) + '/');
      }
      var PACKAGE_NAME = 'build/scripts/wgpu/wgpu_scene.data';
      var REMOTE_PACKAGE_BASE = 'wgpu_scene.data';
      var REMOTE_PACKAGE_NAME = Module['locateFile'] ? Module['locateFile'](REMOTE_PACKAGE_BASE, '') : REMOTE_PACKAGE_BASE;
var REMOTE_PACKAGE_SIZE = metadata['remote_package_size'];

      function fetchRemotePackage(packageName, packageSize, callback, errback) {
        if (isNode) {
          require('fs').readFile(packageName, (err, contents) => {
            if (err) {
              errback(err);
            } else {
              callback(contents.buffer);
            }
          });
          return;
        }
        Module['dataFileDownloads'] ??= {};
        fetch(packageName)
          .catch((cause) => Promise.reject(new Error(`Network Error: ${packageName}`, {cause}))) // If fetch fails, rewrite the error to include the failing URL & the cause.
          .then((response) => {
            if (!response.ok) {
              return Promise.reject(new Error(`${response.status}: ${response.url}`));
            }

            if (!response.body && response.arrayBuffer) { // If we're using the polyfill, readers won't be available...
              return response.arrayBuffer().then(callback);
            }

            const reader = response.body.getReader();
            const iterate = () => reader.read().then(handleChunk).catch((cause) => {
              return Promise.reject(new Error(`Unexpected error while handling : ${response.url} ${cause}`, {cause}));
            });

            const chunks = [];
            const headers = response.headers;
            const total = Number(headers.get('Content-Length') ?? packageSize);
            let loaded = 0;

            const handleChunk = ({done, value}) => {
              if (!done) {
                chunks.push(value);
                loaded += value.length;
                Module['dataFileDownloads'][packageName] = {loaded, total};

                let totalLoaded = 0;
                let totalSize = 0;

                for (const download of Object.values(Module['dataFileDownloads'])) {
                  totalLoaded += download.loaded;
                  totalSize += download.total;
                }

                Module['setStatus']?.(`Downloading data... (${totalLoaded}/${totalSize})`);
                return iterate();
              } else {
                const packageData = new Uint8Array(chunks.map((c) => c.length).reduce((a, b) => a + b, 0));
                let offset = 0;
                for (const chunk of chunks) {
                  packageData.set(chunk, offset);
                  offset += chunk.length;
                }
                callback(packageData.buffer);
              }
            };

            Module['setStatus']?.('Downloading data...');
            return iterate();
          });
      };

      function handleError(error) {
        console.error('package error:', error);
      };

      var fetchedCallback = null;
      var fetched = Module['getPreloadedPackage'] ? Module['getPreloadedPackage'](REMOTE_PACKAGE_NAME, REMOTE_PACKAGE_SIZE) : null;

      if (!fetched) fetchRemotePackage(REMOTE_PACKAGE_NAME, REMOTE_PACKAGE_SIZE, (data) => {
        if (fetchedCallback) {
          fetchedCallback(data);
          fetchedCallback = null;
        } else {
          fetched = data;
        }
      }, handleError);

    function runWithFS(Module) {

      function assert(check, msg) {
        if (!check) throw msg + new Error().stack;
      }
Module['FS_createPath']("/", "resources", true, true);
Module['FS_createPath']("/resources", "assets", true, true);
Module['FS_createPath']("/resources/assets", "gltf", true, true);
Module['FS_createPath']("/", "runtime", true, true);
Module['FS_createPath']("/runtime", "assets", true, true);
Module['FS_createPath']("/runtime/assets", "shader", true, true);

      /** @constructor */
      function DataRequest(start, end, audio) {
        this.start = start;
        this.end = end;
        this.audio = audio;
      }
      DataRequest.prototype = {
        requests: {},
        open: function(mode, name) {
          this.name = name;
          this.requests[name] = this;
          Module['addRunDependency'](`fp ${this.name}`);
        },
        send: function() {},
        onload: function() {
          var byteArray = this.byteArray.subarray(this.start, this.end);
          this.finish(byteArray);
        },
        finish: function(byteArray) {
          var that = this;
          // canOwn this data in the filesystem, it is a slide into the heap that will never change
          Module['FS_createDataFile'](this.name, null, byteArray, true, true, true);
          Module['removeRunDependency'](`fp ${that.name}`);
          this.requests[this.name] = null;
        }
      };

      var files = metadata['files'];
      for (var i = 0; i < files.length; ++i) {
        new DataRequest(files[i]['start'], files[i]['end'], files[i]['audio'] || 0).open('GET', files[i]['filename']);
      }

      function processPackageData(arrayBuffer) {
        assert(arrayBuffer, 'Loading data file failed.');
        assert(arrayBuffer.constructor.name === ArrayBuffer.name, 'bad input to processPackageData');
        var byteArray = new Uint8Array(arrayBuffer);
        var curr;
        // Reuse the bytearray from the XHR as the source for file reads.
          DataRequest.prototype.byteArray = byteArray;
          var files = metadata['files'];
          for (var i = 0; i < files.length; ++i) {
            DataRequest.prototype.requests[files[i].filename].onload();
          }          Module['removeRunDependency']('datafile_build/scripts/wgpu/wgpu_scene.data');

      };
      Module['addRunDependency']('datafile_build/scripts/wgpu/wgpu_scene.data');

      Module['preloadResults'] ??= {};

      Module['preloadResults'][PACKAGE_NAME] = {fromCache: false};
      if (fetched) {
        processPackageData(fetched);
        fetched = null;
      } else {
        fetchedCallback = processPackageData;
      }

    }
    if (Module['calledRun']) {
      runWithFS(Module);
    } else {
      (Module['preRun'] ??= []).push(runWithFS); // FS is not initialized yet, wait for it
    }

    }
    loadPackage({"files": [{"filename": "/resources/assets/gltf/cube.gltf", "start": 0, "end": 7437}, {"filename": "/resources/assets/gltf/ico.gltf", "start": 7437, "end": 19691}, {"filename": "/runtime/assets/shader/shader.default.wgsl", "start": 19691, "end": 21156}, {"filename": "/runtime/assets/shader/shader.grid.wgsl", "start": 21156, "end": 26338}, {"filename": "/runtime/assets/shader/shader.pbr.wgsl", "start": 26338, "end": 28083}], "remote_package_size": 28083});

  })();

// end include: /var/folders/hx/g36pxlqs1dj0kvmzszq_j3k00000gq/T/tmpsatpkwed.js
// include: /var/folders/hx/g36pxlqs1dj0kvmzszq_j3k00000gq/T/tmp8cf_mha4.js

    // All the pre-js content up to here must remain later on, we need to run
    // it.
    if (Module['$ww'] || (typeof ENVIRONMENT_IS_PTHREAD != 'undefined' && ENVIRONMENT_IS_PTHREAD)) Module['preRun'] = [];
    var necessaryPreJSTasks = Module['preRun'].slice();
  // end include: /var/folders/hx/g36pxlqs1dj0kvmzszq_j3k00000gq/T/tmp8cf_mha4.js
// include: /var/folders/hx/g36pxlqs1dj0kvmzszq_j3k00000gq/T/tmp36vo2fjo.js

    if (!Module['preRun']) throw 'Module.preRun should exist because file support used it; did a pre-js delete it?';
    necessaryPreJSTasks.forEach((task) => {
      if (Module['preRun'].indexOf(task) < 0) throw 'All preRun tasks that exist before user pre-js code should remain after; did you replace Module or modify Module.preRun?';
    });
  // end include: /var/folders/hx/g36pxlqs1dj0kvmzszq_j3k00000gq/T/tmp36vo2fjo.js


// Sometimes an existing Module object exists with properties
// meant to overwrite the default module functionality. Here
// we collect those properties and reapply _after_ we configure
// the current environment's defaults to avoid having to be so
// defensive during initialization.
var moduleOverrides = Object.assign({}, Module);

var arguments_ = [];
var thisProgram = './this.program';
var quit_ = (status, toThrow) => {
  throw toThrow;
};

// `/` should be present at the end if `scriptDirectory` is not empty
var scriptDirectory = '';
function locateFile(path) {
  if (Module['locateFile']) {
    return Module['locateFile'](path, scriptDirectory);
  }
  return scriptDirectory + path;
}

// Hooks that are implemented differently in different runtime environments.
var readAsync, readBinary;

if (ENVIRONMENT_IS_NODE) {
  if (typeof process == 'undefined' || !process.release || process.release.name !== 'node') throw new Error('not compiled for this environment (did you build to HTML and try to run it not on the web, or set ENVIRONMENT to something - like node - and run it someplace else - like on the web?)');

  var nodeVersion = process.versions.node;
  var numericVersion = nodeVersion.split('.').slice(0, 3);
  numericVersion = (numericVersion[0] * 10000) + (numericVersion[1] * 100) + (numericVersion[2].split('-')[0] * 1);
  var minVersion = 160000;
  if (numericVersion < 160000) {
    throw new Error('This emscripten-generated code requires node v16.0.0 (detected v' + nodeVersion + ')');
  }

  // These modules will usually be used on Node.js. Load them eagerly to avoid
  // the complexity of lazy-loading.
  var fs = require('fs');
  var nodePath = require('path');

  scriptDirectory = __dirname + '/';

// include: node_shell_read.js
readBinary = (filename) => {
  // We need to re-wrap `file://` strings to URLs.
  filename = isFileURI(filename) ? new URL(filename) : filename;
  var ret = fs.readFileSync(filename);
  assert(Buffer.isBuffer(ret));
  return ret;
};

readAsync = async (filename, binary = true) => {
  // See the comment in the `readBinary` function.
  filename = isFileURI(filename) ? new URL(filename) : filename;
  var ret = fs.readFileSync(filename, binary ? undefined : 'utf8');
  assert(binary ? Buffer.isBuffer(ret) : typeof ret == 'string');
  return ret;
};
// end include: node_shell_read.js
  if (!Module['thisProgram'] && process.argv.length > 1) {
    thisProgram = process.argv[1].replace(/\\/g, '/');
  }

  arguments_ = process.argv.slice(2);

  if (typeof module != 'undefined') {
    module['exports'] = Module;
  }

  quit_ = (status, toThrow) => {
    process.exitCode = status;
    throw toThrow;
  };

} else
if (ENVIRONMENT_IS_SHELL) {

  if ((typeof process == 'object' && typeof require === 'function') || typeof window == 'object' || typeof WorkerGlobalScope != 'undefined') throw new Error('not compiled for this environment (did you build to HTML and try to run it not on the web, or set ENVIRONMENT to something - like node - and run it someplace else - like on the web?)');

} else

// Note that this includes Node.js workers when relevant (pthreads is enabled).
// Node.js workers are detected as a combination of ENVIRONMENT_IS_WORKER and
// ENVIRONMENT_IS_NODE.
if (ENVIRONMENT_IS_WEB || ENVIRONMENT_IS_WORKER) {
  if (ENVIRONMENT_IS_WORKER) { // Check worker, not web, since window could be polyfilled
    scriptDirectory = self.location.href;
  } else if (typeof document != 'undefined' && document.currentScript) { // web
    scriptDirectory = document.currentScript.src;
  }
  // blob urls look like blob:http://site.com/etc/etc and we cannot infer anything from them.
  // otherwise, slice off the final part of the url to find the script directory.
  // if scriptDirectory does not contain a slash, lastIndexOf will return -1,
  // and scriptDirectory will correctly be replaced with an empty string.
  // If scriptDirectory contains a query (starting with ?) or a fragment (starting with #),
  // they are removed because they could contain a slash.
  if (scriptDirectory.startsWith('blob:')) {
    scriptDirectory = '';
  } else {
    scriptDirectory = scriptDirectory.substr(0, scriptDirectory.replace(/[?#].*/, '').lastIndexOf('/')+1);
  }

  if (!(typeof window == 'object' || typeof WorkerGlobalScope != 'undefined')) throw new Error('not compiled for this environment (did you build to HTML and try to run it not on the web, or set ENVIRONMENT to something - like node - and run it someplace else - like on the web?)');

  {
// include: web_or_worker_shell_read.js
if (ENVIRONMENT_IS_WORKER) {
    readBinary = (url) => {
      var xhr = new XMLHttpRequest();
      xhr.open('GET', url, false);
      xhr.responseType = 'arraybuffer';
      xhr.send(null);
      return new Uint8Array(/** @type{!ArrayBuffer} */(xhr.response));
    };
  }

  readAsync = async (url) => {
    // Fetch has some additional restrictions over XHR, like it can't be used on a file:// url.
    // See https://github.com/github/fetch/pull/92#issuecomment-140665932
    // Cordova or Electron apps are typically loaded from a file:// url.
    // So use XHR on webview if URL is a file URL.
    if (isFileURI(url)) {
      return new Promise((resolve, reject) => {
        var xhr = new XMLHttpRequest();
        xhr.open('GET', url, true);
        xhr.responseType = 'arraybuffer';
        xhr.onload = () => {
          if (xhr.status == 200 || (xhr.status == 0 && xhr.response)) { // file URLs can return 0
            resolve(xhr.response);
            return;
          }
          reject(xhr.status);
        };
        xhr.onerror = reject;
        xhr.send(null);
      });
    }
    var response = await fetch(url, { credentials: 'same-origin' });
    if (response.ok) {
      return response.arrayBuffer();
    }
    throw new Error(response.status + ' : ' + response.url);
  };
// end include: web_or_worker_shell_read.js
  }
} else
{
  throw new Error('environment detection error');
}

var out = Module['print'] || console.log.bind(console);
var err = Module['printErr'] || console.error.bind(console);

// Merge back in the overrides
Object.assign(Module, moduleOverrides);
// Free the object hierarchy contained in the overrides, this lets the GC
// reclaim data used.
moduleOverrides = null;
checkIncomingModuleAPI();

// Emit code to handle expected values on the Module object. This applies Module.x
// to the proper local x. This has two benefits: first, we only emit it if it is
// expected to arrive, and second, by using a local everywhere else that can be
// minified.

if (Module['arguments']) arguments_ = Module['arguments'];legacyModuleProp('arguments', 'arguments_');

if (Module['thisProgram']) thisProgram = Module['thisProgram'];legacyModuleProp('thisProgram', 'thisProgram');

// perform assertions in shell.js after we set up out() and err(), as otherwise if an assertion fails it cannot print the message
// Assertions on removed incoming Module JS APIs.
assert(typeof Module['memoryInitializerPrefixURL'] == 'undefined', 'Module.memoryInitializerPrefixURL option was removed, use Module.locateFile instead');
assert(typeof Module['pthreadMainPrefixURL'] == 'undefined', 'Module.pthreadMainPrefixURL option was removed, use Module.locateFile instead');
assert(typeof Module['cdInitializerPrefixURL'] == 'undefined', 'Module.cdInitializerPrefixURL option was removed, use Module.locateFile instead');
assert(typeof Module['filePackagePrefixURL'] == 'undefined', 'Module.filePackagePrefixURL option was removed, use Module.locateFile instead');
assert(typeof Module['read'] == 'undefined', 'Module.read option was removed');
assert(typeof Module['readAsync'] == 'undefined', 'Module.readAsync option was removed (modify readAsync in JS)');
assert(typeof Module['readBinary'] == 'undefined', 'Module.readBinary option was removed (modify readBinary in JS)');
assert(typeof Module['setWindowTitle'] == 'undefined', 'Module.setWindowTitle option was removed (modify emscripten_set_window_title in JS)');
assert(typeof Module['TOTAL_MEMORY'] == 'undefined', 'Module.TOTAL_MEMORY has been renamed Module.INITIAL_MEMORY');
legacyModuleProp('asm', 'wasmExports');
legacyModuleProp('readAsync', 'readAsync');
legacyModuleProp('readBinary', 'readBinary');
legacyModuleProp('setWindowTitle', 'setWindowTitle');
var IDBFS = 'IDBFS is no longer included by default; build with -lidbfs.js';
var PROXYFS = 'PROXYFS is no longer included by default; build with -lproxyfs.js';
var WORKERFS = 'WORKERFS is no longer included by default; build with -lworkerfs.js';
var FETCHFS = 'FETCHFS is no longer included by default; build with -lfetchfs.js';
var ICASEFS = 'ICASEFS is no longer included by default; build with -licasefs.js';
var JSFILEFS = 'JSFILEFS is no longer included by default; build with -ljsfilefs.js';
var OPFS = 'OPFS is no longer included by default; build with -lopfs.js';

var NODEFS = 'NODEFS is no longer included by default; build with -lnodefs.js';

assert(!ENVIRONMENT_IS_SHELL, 'shell environment detected but not enabled at build time.  Add `shell` to `-sENVIRONMENT` to enable.');

// end include: shell.js

// include: preamble.js
// === Preamble library stuff ===

// Documentation for the public APIs defined in this file must be updated in:
//    site/source/docs/api_reference/preamble.js.rst
// A prebuilt local version of the documentation is available at:
//    site/build/text/docs/api_reference/preamble.js.txt
// You can also build docs locally as HTML or other formats in site/
// An online HTML version (which may be of a different version of Emscripten)
//    is up at http://kripken.github.io/emscripten-site/docs/api_reference/preamble.js.html

var wasmBinary = Module['wasmBinary'];legacyModuleProp('wasmBinary', 'wasmBinary');

if (typeof WebAssembly != 'object') {
  err('no native wasm support detected');
}

// Wasm globals

var wasmMemory;

//========================================
// Runtime essentials
//========================================

// whether we are quitting the application. no code should run after this.
// set in exit() and abort()
var ABORT = false;

// set by exit() and abort().  Passed to 'onExit' handler.
// NOTE: This is also used as the process return code code in shell environments
// but only when noExitRuntime is false.
var EXITSTATUS;

// In STRICT mode, we only define assert() when ASSERTIONS is set.  i.e. we
// don't define it at all in release modes.  This matches the behaviour of
// MINIMAL_RUNTIME.
// TODO(sbc): Make this the default even without STRICT enabled.
/** @type {function(*, string=)} */
function assert(condition, text) {
  if (!condition) {
    abort('Assertion failed' + (text ? ': ' + text : ''));
  }
}

// We used to include malloc/free by default in the past. Show a helpful error in
// builds with assertions.
function _free() {
  // Show a helpful error since we used to include free by default in the past.
  abort('free() called but not included in the build - add `_free` to EXPORTED_FUNCTIONS');
}

// Memory management

var HEAP,
/** @type {!Int8Array} */
  HEAP8,
/** @type {!Uint8Array} */
  HEAPU8,
/** @type {!Int16Array} */
  HEAP16,
/** @type {!Uint16Array} */
  HEAPU16,
/** @type {!Int32Array} */
  HEAP32,
/** @type {!Uint32Array} */
  HEAPU32,
/** @type {!Float32Array} */
  HEAPF32,
/* BigInt64Array type is not correctly defined in closure
/** not-@type {!BigInt64Array} */
  HEAP64,
/* BigUint64Array type is not correctly defined in closure
/** not-t@type {!BigUint64Array} */
  HEAPU64,
/** @type {!Float64Array} */
  HEAPF64;

var runtimeInitialized = false;

// include: URIUtils.js
// Prefix of data URIs emitted by SINGLE_FILE and related options.
var dataURIPrefix = 'data:application/octet-stream;base64,';

/**
 * Indicates whether filename is a base64 data URI.
 * @noinline
 */
var isDataURI = (filename) => filename.startsWith(dataURIPrefix);

/**
 * Indicates whether filename is delivered via file protocol (as opposed to http/https)
 * @noinline
 */
var isFileURI = (filename) => filename.startsWith('file://');
// end include: URIUtils.js
// include: runtime_shared.js
// include: runtime_stack_check.js
// Initializes the stack cookie. Called at the startup of main and at the startup of each thread in pthreads mode.
function writeStackCookie() {
  var max = _emscripten_stack_get_end();
  assert((max & 3) == 0);
  // If the stack ends at address zero we write our cookies 4 bytes into the
  // stack.  This prevents interference with SAFE_HEAP and ASAN which also
  // monitor writes to address zero.
  if (max == 0) {
    max += 4;
  }
  // The stack grow downwards towards _emscripten_stack_get_end.
  // We write cookies to the final two words in the stack and detect if they are
  // ever overwritten.
  HEAPU32[((max)>>2)] = 0x02135467;
  HEAPU32[(((max)+(4))>>2)] = 0x89BACDFE;
  // Also test the global address 0 for integrity.
  HEAPU32[((0)>>2)] = 1668509029;
}

function checkStackCookie() {
  if (ABORT) return;
  var max = _emscripten_stack_get_end();
  // See writeStackCookie().
  if (max == 0) {
    max += 4;
  }
  var cookie1 = HEAPU32[((max)>>2)];
  var cookie2 = HEAPU32[(((max)+(4))>>2)];
  if (cookie1 != 0x02135467 || cookie2 != 0x89BACDFE) {
    abort(`Stack overflow! Stack cookie has been overwritten at ${ptrToString(max)}, expected hex dwords 0x89BACDFE and 0x2135467, but received ${ptrToString(cookie2)} ${ptrToString(cookie1)}`);
  }
  // Also test the global address 0 for integrity.
  if (HEAPU32[((0)>>2)] != 0x63736d65 /* 'emsc' */) {
    abort('Runtime error: The application has corrupted its heap memory area (address zero)!');
  }
}
// end include: runtime_stack_check.js
// include: runtime_exceptions.js
// end include: runtime_exceptions.js
// include: runtime_debug.js
// Endianness check
(() => {
  var h16 = new Int16Array(1);
  var h8 = new Int8Array(h16.buffer);
  h16[0] = 0x6373;
  if (h8[0] !== 0x73 || h8[1] !== 0x63) throw 'Runtime error: expected the system to be little-endian! (Run with -sSUPPORT_BIG_ENDIAN to bypass)';
})();

if (Module['ENVIRONMENT']) {
  throw new Error('Module.ENVIRONMENT has been deprecated. To force the environment, use the ENVIRONMENT compile-time option (for example, -sENVIRONMENT=web or -sENVIRONMENT=node)');
}

function legacyModuleProp(prop, newName, incoming=true) {
  if (!Object.getOwnPropertyDescriptor(Module, prop)) {
    Object.defineProperty(Module, prop, {
      configurable: true,
      get() {
        let extra = incoming ? ' (the initial value can be provided on Module, but after startup the value is only looked for on a local variable of that name)' : '';
        abort(`\`Module.${prop}\` has been replaced by \`${newName}\`` + extra);

      }
    });
  }
}

function ignoredModuleProp(prop) {
  if (Object.getOwnPropertyDescriptor(Module, prop)) {
    abort(`\`Module.${prop}\` was supplied but \`${prop}\` not included in INCOMING_MODULE_JS_API`);
  }
}

// forcing the filesystem exports a few things by default
function isExportedByForceFilesystem(name) {
  return name === 'FS_createPath' ||
         name === 'FS_createDataFile' ||
         name === 'FS_createPreloadedFile' ||
         name === 'FS_unlink' ||
         name === 'addRunDependency' ||
         // The old FS has some functionality that WasmFS lacks.
         name === 'FS_createLazyFile' ||
         name === 'FS_createDevice' ||
         name === 'removeRunDependency';
}

/**
 * Intercept access to a global symbol.  This enables us to give informative
 * warnings/errors when folks attempt to use symbols they did not include in
 * their build, or no symbols that no longer exist.
 */
function hookGlobalSymbolAccess(sym, func) {
  if (typeof globalThis != 'undefined' && !Object.getOwnPropertyDescriptor(globalThis, sym)) {
    Object.defineProperty(globalThis, sym, {
      configurable: true,
      get() {
        func();
        return undefined;
      }
    });
  }
}

function missingGlobal(sym, msg) {
  hookGlobalSymbolAccess(sym, () => {
    warnOnce(`\`${sym}\` is not longer defined by emscripten. ${msg}`);
  });
}

missingGlobal('buffer', 'Please use HEAP8.buffer or wasmMemory.buffer');
missingGlobal('asm', 'Please use wasmExports instead');

function missingLibrarySymbol(sym) {
  hookGlobalSymbolAccess(sym, () => {
    // Can't `abort()` here because it would break code that does runtime
    // checks.  e.g. `if (typeof SDL === 'undefined')`.
    var msg = `\`${sym}\` is a library symbol and not included by default; add it to your library.js __deps or to DEFAULT_LIBRARY_FUNCS_TO_INCLUDE on the command line`;
    // DEFAULT_LIBRARY_FUNCS_TO_INCLUDE requires the name as it appears in
    // library.js, which means $name for a JS name with no prefix, or name
    // for a JS name like _name.
    var librarySymbol = sym;
    if (!librarySymbol.startsWith('_')) {
      librarySymbol = '$' + sym;
    }
    msg += ` (e.g. -sDEFAULT_LIBRARY_FUNCS_TO_INCLUDE='${librarySymbol}')`;
    if (isExportedByForceFilesystem(sym)) {
      msg += '. Alternatively, forcing filesystem support (-sFORCE_FILESYSTEM) can export this for you';
    }
    warnOnce(msg);
  });

  // Any symbol that is not included from the JS library is also (by definition)
  // not exported on the Module object.
  unexportedRuntimeSymbol(sym);
}

function unexportedRuntimeSymbol(sym) {
  if (!Object.getOwnPropertyDescriptor(Module, sym)) {
    Object.defineProperty(Module, sym, {
      configurable: true,
      get() {
        var msg = `'${sym}' was not exported. add it to EXPORTED_RUNTIME_METHODS (see the Emscripten FAQ)`;
        if (isExportedByForceFilesystem(sym)) {
          msg += '. Alternatively, forcing filesystem support (-sFORCE_FILESYSTEM) can export this for you';
        }
        abort(msg);
      }
    });
  }
}

// Used by XXXXX_DEBUG settings to output debug messages.
function dbg(...args) {
  // TODO(sbc): Make this configurable somehow.  Its not always convenient for
  // logging to show up as warnings.
  console.warn(...args);
}
// end include: runtime_debug.js
// include: memoryprofiler.js
// end include: memoryprofiler.js
// include: base64Decode.js
// Precreate a reverse lookup table from chars "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/" back to bytes to make decoding fast.
for (var base64ReverseLookup = new Uint8Array(123/*'z'+1*/), i = 25; i >= 0; --i) {
  base64ReverseLookup[48+i] = 52+i; // '0-9'
  base64ReverseLookup[65+i] = i; // 'A-Z'
  base64ReverseLookup[97+i] = 26+i; // 'a-z'
}
base64ReverseLookup[43] = 62; // '+'
base64ReverseLookup[47] = 63; // '/'

// Decodes a _known valid_ base64 string (without validation) and returns it as a new Uint8Array.
// Benchmarked to be around 5x faster compared to a simple
// "Uint8Array.from(atob(b64), c => c.charCodeAt(0))" (TODO: perhaps use this form in -Oz builds?)
/** @noinline */
function base64Decode(b64) {
  if (typeof ENVIRONMENT_IS_NODE != 'undefined' && ENVIRONMENT_IS_NODE) {
    var buf = Buffer.from(b64, 'base64');
    return new Uint8Array(buf.buffer, buf.byteOffset, buf.length);
  }

  assert(b64.length % 4 == 0);
  var b1, b2, i = 0, j = 0, bLength = b64.length, output = new Uint8Array((bLength*3>>2) - (b64[bLength-2] == '=') - (b64[bLength-1] == '='));
  for (; i < bLength; i += 4, j += 3) {
    b1 = base64ReverseLookup[b64.charCodeAt(i+1)];
    b2 = base64ReverseLookup[b64.charCodeAt(i+2)];
    output[j] = base64ReverseLookup[b64.charCodeAt(i)] << 2 | b1 >> 4;
    output[j+1] = b1 << 4 | b2 >> 2;
    output[j+2] = b2 << 6 | base64ReverseLookup[b64.charCodeAt(i+3)];
  }
  return output;
}

// If filename is a base64 data URI, parses and returns data (Buffer on node,
// Uint8Array otherwise). If filename is not a base64 data URI, returns undefined.
function tryParseAsDataURI(filename) {
  if (!isDataURI(filename)) {
    return;
  }

  return base64Decode(filename.slice(dataURIPrefix.length));
}
// end include: base64Decode.js


function updateMemoryViews() {
  var b = wasmMemory.buffer;
  Module['HEAP8'] = HEAP8 = new Int8Array(b);
  Module['HEAP16'] = HEAP16 = new Int16Array(b);
  Module['HEAPU8'] = HEAPU8 = new Uint8Array(b);
  Module['HEAPU16'] = HEAPU16 = new Uint16Array(b);
  Module['HEAP32'] = HEAP32 = new Int32Array(b);
  Module['HEAPU32'] = HEAPU32 = new Uint32Array(b);
  Module['HEAPF32'] = HEAPF32 = new Float32Array(b);
  Module['HEAPF64'] = HEAPF64 = new Float64Array(b);
  Module['HEAP64'] = HEAP64 = new BigInt64Array(b);
  Module['HEAPU64'] = HEAPU64 = new BigUint64Array(b);
}

// end include: runtime_shared.js
assert(!Module['STACK_SIZE'], 'STACK_SIZE can no longer be set at runtime.  Use -sSTACK_SIZE at link time')

assert(typeof Int32Array != 'undefined' && typeof Float64Array !== 'undefined' && Int32Array.prototype.subarray != undefined && Int32Array.prototype.set != undefined,
       'JS engine does not provide full typed array support');

// If memory is defined in wasm, the user can't provide it, or set INITIAL_MEMORY
assert(!Module['wasmMemory'], 'Use of `wasmMemory` detected.  Use -sIMPORTED_MEMORY to define wasmMemory externally');
assert(!Module['INITIAL_MEMORY'], 'Detected runtime INITIAL_MEMORY setting.  Use -sIMPORTED_MEMORY to define wasmMemory dynamically');

var __ATPRERUN__  = []; // functions called before the runtime is initialized
var __ATINIT__    = []; // functions called during startup
var __ATMAIN__    = []; // functions called when main() is to be run
var __ATEXIT__    = []; // functions called during shutdown
var __ATPOSTRUN__ = []; // functions called after the main() is called

function preRun() {
  if (Module['preRun']) {
    if (typeof Module['preRun'] == 'function') Module['preRun'] = [Module['preRun']];
    while (Module['preRun'].length) {
      addOnPreRun(Module['preRun'].shift());
    }
  }
  callRuntimeCallbacks(__ATPRERUN__);
}

function initRuntime() {
  assert(!runtimeInitialized);
  runtimeInitialized = true;

  checkStackCookie();

  
if (!Module['noFSInit'] && !FS.initialized)
  FS.init();
FS.ignorePermissions = false;

TTY.init();
  callRuntimeCallbacks(__ATINIT__);
}

function preMain() {
  checkStackCookie();
  
  callRuntimeCallbacks(__ATMAIN__);
}

function postRun() {
  checkStackCookie();

  if (Module['postRun']) {
    if (typeof Module['postRun'] == 'function') Module['postRun'] = [Module['postRun']];
    while (Module['postRun'].length) {
      addOnPostRun(Module['postRun'].shift());
    }
  }

  callRuntimeCallbacks(__ATPOSTRUN__);
}

function addOnPreRun(cb) {
  __ATPRERUN__.unshift(cb);
}

function addOnInit(cb) {
  __ATINIT__.unshift(cb);
}

function addOnPreMain(cb) {
  __ATMAIN__.unshift(cb);
}

function addOnExit(cb) {
}

function addOnPostRun(cb) {
  __ATPOSTRUN__.unshift(cb);
}

// A counter of dependencies for calling run(). If we need to
// do asynchronous work before running, increment this and
// decrement it. Incrementing must happen in a place like
// Module.preRun (used by emcc to add file preloading).
// Note that you can add dependencies in preRun, even though
// it happens right before run - run will be postponed until
// the dependencies are met.
var runDependencies = 0;
var dependenciesFulfilled = null; // overridden to take different actions when all run dependencies are fulfilled
var runDependencyTracking = {};
var runDependencyWatcher = null;

function getUniqueRunDependency(id) {
  var orig = id;
  while (1) {
    if (!runDependencyTracking[id]) return id;
    id = orig + Math.random();
  }
}

function addRunDependency(id) {
  runDependencies++;

  Module['monitorRunDependencies']?.(runDependencies);

  if (id) {
    assert(!runDependencyTracking[id]);
    runDependencyTracking[id] = 1;
    if (runDependencyWatcher === null && typeof setInterval != 'undefined') {
      // Check for missing dependencies every few seconds
      runDependencyWatcher = setInterval(() => {
        if (ABORT) {
          clearInterval(runDependencyWatcher);
          runDependencyWatcher = null;
          return;
        }
        var shown = false;
        for (var dep in runDependencyTracking) {
          if (!shown) {
            shown = true;
            err('still waiting on run dependencies:');
          }
          err(`dependency: ${dep}`);
        }
        if (shown) {
          err('(end of list)');
        }
      }, 10000);
    }
  } else {
    err('warning: run dependency added without ID');
  }
}

function removeRunDependency(id) {
  runDependencies--;

  Module['monitorRunDependencies']?.(runDependencies);

  if (id) {
    assert(runDependencyTracking[id]);
    delete runDependencyTracking[id];
  } else {
    err('warning: run dependency removed without ID');
  }
  if (runDependencies == 0) {
    if (runDependencyWatcher !== null) {
      clearInterval(runDependencyWatcher);
      runDependencyWatcher = null;
    }
    if (dependenciesFulfilled) {
      var callback = dependenciesFulfilled;
      dependenciesFulfilled = null;
      callback(); // can add another dependenciesFulfilled
    }
  }
}

/** @param {string|number=} what */
function abort(what) {
  Module['onAbort']?.(what);

  what = 'Aborted(' + what + ')';
  // TODO(sbc): Should we remove printing and leave it up to whoever
  // catches the exception?
  err(what);

  ABORT = true;

  // Use a wasm runtime error, because a JS error might be seen as a foreign
  // exception, which means we'd run destructors on it. We need the error to
  // simply make the program stop.
  // FIXME This approach does not work in Wasm EH because it currently does not assume
  // all RuntimeErrors are from traps; it decides whether a RuntimeError is from
  // a trap or not based on a hidden field within the object. So at the moment
  // we don't have a way of throwing a wasm trap from JS. TODO Make a JS API that
  // allows this in the wasm spec.

  // Suppress closure compiler warning here. Closure compiler's builtin extern
  // definition for WebAssembly.RuntimeError claims it takes no arguments even
  // though it can.
  // TODO(https://github.com/google/closure-compiler/pull/3913): Remove if/when upstream closure gets fixed.
  /** @suppress {checkTypes} */
  var e = new WebAssembly.RuntimeError(what);

  // Throw the error whether or not MODULARIZE is set because abort is used
  // in code paths apart from instantiation where an exception is expected
  // to be thrown when abort is called.
  throw e;
}

function createExportWrapper(name, nargs) {
  return (...args) => {
    assert(runtimeInitialized, `native function \`${name}\` called before runtime initialization`);
    var f = wasmExports[name];
    assert(f, `exported native function \`${name}\` not found`);
    // Only assert for too many arguments. Too few can be valid since the missing arguments will be zero filled.
    assert(args.length <= nargs, `native function \`${name}\` called with ${args.length} args but expects ${nargs}`);
    return f(...args);
  };
}

// In SINGLE_FILE mode the wasm binary is encoded inline here as a data: URL.
var wasmBinaryFile = 'data:application/octet-stream;base64,AGFzbQEAAAABwgIxYAJ/fwF/YAJ/fwBgA39/fwBgBX9/f39/AX9gAX8Bf2ADf39/AX9gA39+fwF+YAZ/fH9/f38Bf2AEf39/fwBgAX8AYAV/f35/fwBgBX9/f39/AGAFf39/fn4AYAZ/f39/f38AYAABf2ADf3x8AX9gA39+fwF/YAR/f39/AX9gBH9+f38Bf2AAAGAGf39/f39/AX9gB39/f39/f38Bf2ACf38BfWADf399AGADf319AGABfwF8YAF/AX5gAnx/AX9gAXwBfWACfX8Bf2ABfQF9YAF8AXxgAnx/AXxgAn9+AGAFf35+fn4AYAR/fn5/AGACfn4Bf2ADf35+AGAHf39/f39/fwBgAn9/AX5gAn9/AXxgA3x8fwF8YAN+f38Bf2ACfn8Bf2ABfAF+YAR+fn5+AX9gAn98AGACf30AYAJ+fgF8AqwONwNlbnYNX19hc3NlcnRfZmFpbAAIA2VudgRleGl0AAkDZW52KWVtc2NyaXB0ZW5fc2V0X2tleWRvd25fY2FsbGJhY2tfb25fdGhyZWFkAAMDZW52J2Vtc2NyaXB0ZW5fc2V0X2tleXVwX2NhbGxiYWNrX29uX3RocmVhZAADA2VuditlbXNjcmlwdGVuX3NldF9tb3VzZW1vdmVfY2FsbGJhY2tfb25fdGhyZWFkAAMDZW52J2Vtc2NyaXB0ZW5fc2V0X3doZWVsX2NhbGxiYWNrX29uX3RocmVhZAADA2Vudh93Z3B1RGV2aWNlQ3JlYXRlQmluZEdyb3VwTGF5b3V0AAADZW52HndncHVEZXZpY2VDcmVhdGVQaXBlbGluZUxheW91dAAAA2Vudh53Z3B1RGV2aWNlQ3JlYXRlUmVuZGVyUGlwZWxpbmUAAANlbnYkd2dwdVJlbmRlclBpcGVsaW5lR2V0QmluZEdyb3VwTGF5b3V0AAADZW52GXdncHVEZXZpY2VDcmVhdGVCaW5kR3JvdXAAAANlbnYad2dwdUJpbmRHcm91cExheW91dFJlbGVhc2UACQNlbnYZd2dwdVJlbmRlclBpcGVsaW5lUmVsZWFzZQAJA2Vudhl3Z3B1UGlwZWxpbmVMYXlvdXRSZWxlYXNlAAkDZW52F3dncHVTaGFkZXJNb2R1bGVSZWxlYXNlAAkDZW52IHdncHVSZW5kZXJQYXNzRW5jb2RlclNldFBpcGVsaW5lAAEDZW52FHdncHVRdWV1ZVdyaXRlQnVmZmVyAAoDZW52IXdncHVSZW5kZXJQYXNzRW5jb2RlclNldEJpbmRHcm91cAALA2VudiR3Z3B1UmVuZGVyUGFzc0VuY29kZXJTZXRWZXJ0ZXhCdWZmZXIADANlbnYjd2dwdVJlbmRlclBhc3NFbmNvZGVyU2V0SW5kZXhCdWZmZXIADANlbnYgd2dwdVJlbmRlclBhc3NFbmNvZGVyRHJhd0luZGV4ZWQADQNlbnYcd2dwdURldmljZUNyZWF0ZVNoYWRlck1vZHVsZQAAA2VudhZ3Z3B1RGV2aWNlQ3JlYXRlQnVmZmVyAAADZW52HGVtc2NyaXB0ZW5fd2ViZ3B1X2dldF9kZXZpY2UADgNlbnYSd2dwdURldmljZUdldFF1ZXVlAAQDZW52HmVtc2NyaXB0ZW5fcmVxdWVzdF9wb2ludGVybG9jawAAA2VudihlbXNjcmlwdGVuX3NldF9yZXNpemVfY2FsbGJhY2tfb25fdGhyZWFkAAMDZW52H2Vtc2NyaXB0ZW5fZ2V0X2VsZW1lbnRfY3NzX3NpemUABQNlbnYfZW1zY3JpcHRlbl9zZXRfZWxlbWVudF9jc3Nfc2l6ZQAPA2VudhR3Z3B1U3dhcENoYWluUmVsZWFzZQAJA2VudhB3Z3B1UXVldWVSZWxlYXNlAAkDZW52EXdncHVEZXZpY2VSZWxlYXNlAAkDZW52IndncHVTd2FwQ2hhaW5HZXRDdXJyZW50VGV4dHVyZVZpZXcABANlbnYed2dwdURldmljZUNyZWF0ZUNvbW1hbmRFbmNvZGVyAAADZW52IXdncHVDb21tYW5kRW5jb2RlckJlZ2luUmVuZGVyUGFzcwAAA2Vudhh3Z3B1UmVuZGVyUGFzc0VuY29kZXJFbmQACQNlbnYYd2dwdUNvbW1hbmRFbmNvZGVyRmluaXNoAAADZW52D3dncHVRdWV1ZVN1Ym1pdAACA2Vudhx3Z3B1UmVuZGVyUGFzc0VuY29kZXJSZWxlYXNlAAkDZW52GXdncHVDb21tYW5kRW5jb2RlclJlbGVhc2UACQNlbnYYd2dwdUNvbW1hbmRCdWZmZXJSZWxlYXNlAAkDZW52FndncHVUZXh0dXJlVmlld1JlbGVhc2UACQNlbnYYZW1zY3Jp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KAIUIX8gBygCECGAAUEUIYEBIIABIIEBbCGCASB/IIIBaiGDASAHKAIMIYQBQcCFhIAAIYUBIIMBIIQBIIUBENqAgIAAIYYBAkACQCCGAQ0AIAcoAhghhwEgBygCFCGIASAHKAIQIYkBQQEhigEgiQEgigFqIYsBIAcoAgwhjAEgBygCCCGNAUEQIY4BII0BII4BaiGPASCHASCIASCLASCMASCPARDqgICAACGQASAHIJABNgIQDAELIAcoAhQhkQEgBygCECGSAUEUIZMBIJIBIJMBbCGUASCRASCUAWohlQEgBygCDCGWAUGjhISAACGXASCVASCWASCXARDagICAACGYAQJAAkAgmAENACAHKAIYIZkBIAcoAhQhmgEgBygCECGbASAHKAIMIZwBIAcoAgghnQFBHCGeASCdASCeAWohnwEgBygCCCGgAUEgIaEBIKABIKEBaiGiASCZASCaASCbASCcASCfASCiARDzgICAACGjASAHIKMBNgIQDAELIAcoAhQhpAEgBygCECGlAUEBIaYBIKUBIKYBaiGnASCkASCnARDtgICAACGoASAHIKgBNgIQCwsLCwsLIAcoAhAhqQFBACGqASCpASCqAUghqwFBASGsASCrASCsAXEhrQECQCCtAUUNACAHKAIQIa4BIAcgrgE2AhwMAwsgBygCACGvAUEBIbABIK8BILABaiGxASAHILEBNgIADAALCyAHKAIQIbIBIAcgsgE2AhwLIAcoAhwhswFBICG0ASAHILQBaiG1ASC1ASSAgICAACCzAQ8LkiEBsAN/I4CAgIAAIQVBwAAhBiAFIAZrIQcgBySAgICAACAHIAA2AjggByABNgI0IAcgAjYCMCAHIAM2AiwgByAENgIoIAcoAjQhCCAHKAIwIQlBFCEKIAkgCmwhCyAIIAtqIQwgDCgCACENQQEhDiANIA5HIQ9BASEQIA8gEHEhEQJAAkAgEUUNAEF/IRIgByASNgI8DAELIAcoAjQhEyAHKAIwIRRBFCEVIBQgFWwhFiATIBZqIRcgFygCDCEYIAcgGDYCJCAHKAIwIRlBASEaIBkgGmohGyAHIBs2AjBBACEcIAcgHDYCIAJAA0AgBygCICEdIAcoAiQhHiAdIB5IIR9BASEgIB8gIHEhISAhRQ0BIAcoAjQhIiAHKAIwISNBFCEkICMgJGwh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IKQBIKUBENqAgIAAIaYBAkACQCCmAQ0AIAcoAhghpwEgBygCFCGoASAHKAIQIakBQQEhqgEgqQEgqgFqIasBIAcoAgwhrAEgBygCCCGtAUEUIa4BIK0BIK4BaiGvASCnASCoASCrASCsASCvARDqgICAACGwASAHILABNgIQDAELIAcoAhQhsQEgBygCECGyAUEUIbMBILIBILMBbCG0ASCxASC0AWohtQEgBygCDCG2AUGjhISAACG3ASC1ASC2ASC3ARDagICAACG4AQJAAkAguAENACAHKAIYIbkBIAcoAhQhugEgBygCECG7ASAHKAIMIbwBIAcoAgghvQFBICG+ASC9ASC+AWohvwEgBygCCCHAAUEkIcEBIMABIMEBaiHCASC5ASC6ASC7ASC8ASC/ASDCARDzgICAACHDASAHIMMBNgIQDAELIAcoAhQhxAEgBygCECHFAUEBIcYBIMUBIMYBaiHHASDEASDHARDtgICAACHIASAHIMgBNgIQCwsLCwsLCyAHKAIQIckBQQAhygEgyQEgygFIIcsBQQEhzAEgywEgzAFxIc0BAkAgzQFFDQAgBygCECHOASAHIM4BNgIcDAMLIAcoAgAhzwFBASHQASDPASDQAWoh0QEgByDRATYCAAwACwsgBygCECHSASAHINIBNgIcCyAHKAIcIdMBQSAh1AEgByDUAWoh1QEg1QEkgICAgAAg0wEPC/MRAfMBfyOAgICAACEFQTAhBiAFIAZrIQcgBySAgICAACAHIAA2AiggByABNgIkIAcgAjYCICAHIAM2AhwgByAENgIYIAcoAiQhCCAHKAIgIQlBFCEKIAkgCmwhCyAIIAtqIQwgDCgCACENQQEhDiANIA5HIQ9BASEQIA8gEHEhEQJAAkAgEUUNAEF/IRIgByASNgIsDAELIAcoAiQhEyAHKAIgIRRBFCEVIBQgFWwhFiATIBZqIRcgFygCDCEYIAcgGDYCFCAHKAIgIRlBASEaIBkgGmohGyAHIBs2AiBBACEcIAcgHDYCEAJAA0AgBygCECEdIAcoAhQhHiAdIB5IIR9BASEgIB8gIHEhISAhRQ0BIAcoAiQhIiAHKAIgISNBFCEkICMgJGwhJSAiICVqISYgJigCACEnQQMhKCAnIChHISlBASEqICkgKnEhKwJAAkAgKw0AIAcoAiQhLCAHKAIg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AgQhXCBaIFyUIV0gViBYlCFeIF4gXZIhXyADKALcAyFgIGAqAgghYSADKALYAyFiIGIqAgghYyBhIGOUIWQgZCBfkiFlIGWRIWYgAyBmOAK0AyADKgK0AyFnQwAAADQhaCBnIGhdIWlBASFqIGkganEhawJAAkAga0UNACADKAK4AyFsIAMgbDYCwAMgAygCwAMhbUEAIW4gbrIhbyBtIG84AgggAygCwAMhcEEAIXEgcbIhciBwIHI4AgQgAygCwAMhc0EAIXQgdLIhdSBzIHU4AgAMAQsgAygCvAMhdiADKgK0AyF3QwAAgD8heCB4IHeVIXkgAygCuAMheiADIHY2AswDIAMgeTgCyAMgAyB6NgLEAyADKALMAyF7IHsqAgAhfCADKgLIAyF9IHwgfZQhfiADKALEAyF/IH8gfjgCACADKALMAyGAASCAASoCBCGBASADKgLIAyGCASCBASCCAZQhgwEgAygCxAMhhAEghAEggwE4AgQgAygCzAMhhQEghQEqAgghhgEgAyoCyAMhhwEghgEghwGUIYgBIAMoAsQDIYkBIIkBIIgBOAIICyADKgKsASGKASADKgK4ASGLASCKASCLAZQhjAEgAygCzAEhjQEgjQEgjAE4AgAgAyoCrAEhjgEgAyoCvAEhjwEgjgEgjwGUIZABIAMoAswBIZEBIJEBIJABOAIEIAMqAqwBIZIBIAMqAsABIZMBIJIBIJMBlCGUASADKALMASGVASCVASCUATgCCCADKgKwASGWASADKALMASGXASCXASCWATgCDEHAACGYASADIJgBaiGZASCZASGaASADKgJoIZsBQQAhnAEgnAGyIZ0BIAMgnQE4AhhDAACAPyGeASADIJ4BOAIcQQAhnwEgnwGyIaABIAMgoAE4AiBBGCGhASADIKEBaiGiASCiASGjASADIJoBNgKoASADIJsBOAKkASADIKMBNgKgASADKgKkASGkAUMAAAA/IaUBIKQBIKUBlCGmASADIKYBOAKMASADKgKMASGnASCnARD2gYCAACGoASADIKgBOAKIASADKgKMASGpASCpARCqgoCAACGqASADIKoBOAKEASADKAKgASGrASADIKsBNgLkA0GQASGsASADIKwBaiGtASCtASGuASADIK4BNgLgAyADKALkAyGvASADKALgAyGwASADIK8BNgLw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gAALIAMgAnIL+wIBA38CQCAADQBBACEBAkBBACgCuMqEgABFDQBBACgCuMqEgAAQ+4GAgAAhAQsCQEEAKAKgyYSAAEUNAEEAKAKgyYSAABD7gYCAACABciEBCwJAEJiCgIAAKAIAIgBFDQADQAJAAkAgACgCTEEATg0AQQEhAgwBCyAAEPeBgIAARSECCwJAIAAoAhQgACgCHEYNACAAEPuBgIAAIAFyIQELAkAgAg0AIAAQ+IGAgAALIAAoAjgiAA0ACwsQmYKAgAAgAQ8LAkACQCAAKAJMQQBODQBBASECDAELIAAQ94GAgABFIQILAkACQAJAIAAoAhQgACgCHEYNACAAQQBBACAAKAIkEYWAgIAAgICAgAAaIAAoAhQNAEF/IQEgAkUNAQwCCwJAIAAoAgQiASAAKAIIIgNGDQAgACABIANrrEEBIAAoAigRhoCAgACAgICAABoLQQAhASAAQQA2AhwgAEIANwMQIABCADcCBCACDQELIAAQ+IGAgAALIAELBQAgAJwLfQEBf0ECIQECQCAAQSsQroKAgAANACAALQAAQfIARyEBCyABQYABciABIABB+AAQroKAgAAbIgFBgIAgciABIABB5QAQroKAgAAbIgEgAUHAAHIgAC0AACIAQfIARhsiAUGABHIgASAAQfcARhsiAUGACHIgASAAQeEARhsL8gICA38BfgJAIAJFDQAgACABOgAAIAAgAmoiA0F/aiABOgAAIAJBA0kNACAAIAE6AAIgACABOgABIANBfWogAToAACADQX5qIAE6AAAgAkEHSQ0AIAAgAToAAyADQXxqIAE6AAAgAkEJSQ0AIABBACAAa0EDcSIEaiIDIAFB/wFxQYGChAhsIgE2AgAgAyACIARrQXxxIgRqIgJBfGogATYCACAEQQlJDQAgAyABNgIIIAMgATYCBCACQXhqIAE2AgAgAkF0aiABNgIAIARBGUkNACADIAE2AhggAyABNgIUIAMgATYCECADIAE2AgwgAkFwaiABNgIAIAJBbGogATYCACACQWhqIAE2AgAgAkFkaiABNgIAIAQgA0EEcUEYciIFayICQSBJDQAgAa1CgYCAgBB+IQYgAyAFaiEBA0AgASAGNwMYIAEgBjcDECABIAY3AwggASAGNwMAIAFBIGohASACQWBqIgJBH0sNAAsLIAALEQAgACgCPCABIAIQlIKAgAAL/wIBB38jgICAgABBIGsiAySAgICAACADIAAoAhwiBDYCECAAKAIUIQUgAyACNgIcIAMgATYCGCADIAUgBGsiATYCFCABIAJqIQYgA0EQaiEEQQIhBwJAAkACQAJAAkAgACgCPCADQRBqQQIgA0EMahCxgICAABDlgoCAAEUNACAEIQUMAQsDQCAGIAMoAgwiAUYNAgJAIAFBf0oNACAEIQUMBAsgBCABIAQoAgQiCEsiCUEDdGoiBSAFKAIAIAEgCEEAIAkbayIIajYCACAEQQxBBCAJG2oiBCAEKAIAIAhrNgIAIAYgAWshBiAFIQQgACgCPCAFIAcgCWsiByADQQxqELGAgIAAEOWCgIAARQ0ACwsgBkF/Rw0BCyAAIAAoAiwiATYCHCAAIAE2AhQgACABIAAoAjBqNgIQIAIhAQwBC0EAIQEgAEEANgIcIABCADcDECAAIAAoAgBBIHI2AgAgB0ECRg0AIAIgBSgCBGshAQsgA0EgaiSAgICAACABC/YBAQR/I4CAgIAAQSBrIgMkgICAgAAgAyABNgIQQQAhBCADIAIgACgCMCIFQQBHazYCFCAAKAIsIQYgAyAFNgIcIAMgBjYCGEEgIQUCQAJAAkAgACgCPCADQRBqQQIgA0EMahCygICAABDlgoCAAA0AIAMoAgwiBUEASg0BQSBBECAFGyEFCyAAIAAoAgAgBXI2AgAMAQsgBSEEIAUgAygCFCIGTQ0AIAAgACgCLCIENgIEIAAgBCAFIAZrajYCCAJAIAAoAjBFDQAgACAEQQFqNgIEIAEgAmpBf2ogBC0AADoAAAsgAiEECyADQSBqJICAgIAAIAQLBAAgAAsZACAAKAI8EIKCgIAAELOAgIAAEOWCgIAAC4YDAQJ/I4CAgIAAQSBrIgIkgICAgAACQAJAAkACQEHElYSAACABLAAAEK6CgIAADQAQ74GAgABBHDYCAAwBC0GYCRDqgoCAACIDDQELQQAhAwwBCyADQQBBkAEQ/oGAgAAaAkAgAUErEK6CgIAADQAgA0EIQQQgAS0AAEHyAEYbNgIACwJAAkAgAS0AAEHhAEYNACADKAIAIQEMAQsCQCAAQQNBABCvgICAACIBQYAIcQ0AIAIgAUGACHKsNwMQIABBBCACQRBqEK+AgIAAGgsgAyADKAIAQYABciIBNgIACyADQX82AlAgA0GACDYCMCADIAA2AjwgAyADQZgBajYCLAJAIAFBCHENACACIAJBGGqtNwMAIABBk6gBIAIQsICAgAANACADQQo2AlALIANBjICAgAA2AiggA0GNgICAADYCJCADQY6AgIAANgIgIANBj4CAgAA2AgwCQEEALQC1zoSAAA0AIANBfzYCTAsgAxCagoCAACEDCyACQSBqJICAgIAAIAMLnQEBA38jgICAgABBEGsiAiSAgICAAAJAAkACQEHElYSAACABLAAAEK6CgIAADQAQ74GAgABBHDYCAAwBCyABEP2BgIAAIQMgAkK2AzcDAEEAIQRBnH8gACADQYCAAnIgAhCugICAABDRgoCAACIAQQBIDQEgACABEISCgIAAIgQNASAAELOAgIAAGgtBACEECyACQRBqJICAgIAAIAQLJAEBfyAAELWCgIAAIQJBf0EAIAIgAEEBIAIgARCTgoCAAEcbCxMAIAIEQCAAIAEgAvwKAAALIAALkQQBA38CQCACQYAESQ0AIAAgASACEIeCgIAADwsgACACaiEDAkACQCABIABzQQNxDQACQAJAIABBA3ENACAAIQIMAQsCQCACDQAgACECDAELIAAhAgNAIAIgAS0AADoAACABQQFqIQEgAkEBaiICQQNxRQ0BIAIgA0kNAAsLIANBfHEhBAJAIANBwABJDQAgAiAEQUBqIgVLDQADQCACIAEoAgA2AgAgAiABKAIENgIEIAIgASgCCDYCCCACIAEoAgw2AgwgAiABKAIQNgIQIAIgASgCFDYCFCACIAEoAhg2AhggAiABKAIcNgIcIAIgASgCIDYCICACIAEoAiQ2AiQgAiABKAIoNgIoIAIgASgCLDYCLCACIAEoAjA2AjAgAiABKAI0NgI0IAIgASgCODYCOCACIAEoAjw2AjwgAUHAAGohASACQcAAaiICIAVNDQALCyACIARPDQEDQCACIAEoAgA2AgAgAUEEaiEBIAJBBGoiAiAESQ0ADAILCwJAIANBBE8NACAAIQIMAQsCQCAAIANBfGoiBE0NACAAIQIMAQsgACECA0AgAiABLQAAOgAAIAIgAS0AAToAASACIAEtAAI6AAIgAiABLQADOgADIAFBBGohASACQQRqIgIgBE0NAAsLAkAgAiADTw0AA0AgAiABLQAAOgAAIAFBAWohASAC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QQhqNgIAIAAgASsDADkDAA8LIAAgAiADEYGAgIAAgICAgAALC0ABAX8CQCAAUA0AA0AgAUF/aiIBIACnQQ9xQcDGhIAAai0AACACcjoAACAAQg9WIQMgAEIEiCEAIAMNAAsLIAELNgEBfwJAIABQDQADQCABQX9qIgEgAKdBB3FBMHI6AAAgAEIHViECIABCA4ghACACDQALCyABC4oBAgF+A38CQAJAIABCgICAgBBaDQAgACECDAELA0AgAUF/aiIBIAAgAEIKgCICQgp+fadBMHI6AAAgAEL/////nwFWIQMgAiEAIAMNAAsLAkAgAlANACACpyEDA0AgAUF/aiIBIAMgA0EKbiIEQQpsa0EwcjoAACADQQlLIQUgBCEDIAUNAAsLIAELhAEBAX8jgICAgABBgAJrIgUkgICAgAACQCACIANMDQAgBEGAwARxDQAgBSABIAIgA2siA0GAAiADQYACSSICGxD+gYCAABoCQCACDQADQCAAIAVBgAIQ2IKAgAAgA0GAfmoiA0H/AUsNAAsLIAAgBSADENiCgIAACyAFQYACaiSAgICAAAsaACAAIAEgAkGSgICAAEGTgICAABDWgoCAAAvIGQYCfwF+DH8CfgR/AXwjgICAgABBsARrIgYkgICAgABBACEHIAZBADYCLAJAAkAgARDigoCAACIIQn9VDQBBASEJQc2BhIAAIQogAZoiARDigoCAACEIDAELAkAgBEGAEHFFDQBBASEJQdCBhIAAIQoMAQtB04GEgABBzoGEgAAgBEEBcSIJGyEKIAlFIQcLAkACQCAIQoCAgICAgID4/wCDQoCAgICAgID4/wBSDQAgAEEgIAIgCUEDaiILIARB//97cRDegoCAACAAIAogCRDYgoCAACAAQZiMhIAAQbyWhIAAIAVBIHEiDBtBhY+EgABB45aEgAAgDBsgASABYhtBAxDYgoCAACAAQSAgAiALIARBgMAAcxDegoCAACACIAsgAiALShshDQwBCyAGQRBqIQ4CQAJAAkACQCABIAZBLGoQ1YKAgAAiASABoCIBRAAAAAAAAAAAYQ0AIAYgBigCLCILQX9qNgIsIAVBIHIiD0HhAEcNAQwDCyAFQSByIg9B4QBGDQJBBiADIANBAEgbIRAgBigCLCERDAELIAYgC0FjaiIRNgIsQQYgAyADQQBIGyEQIAFEAAAAAAAAsEGiIQELIAZBMGpBAEGgAiARQQBIG2oiEiEMA0AgDCAB/AMiCzYCACAMQQRqIQwgASALuKFEAAAAAGXNzUGiIgFEAAAAAAAAAABiDQALAkACQCARQQFODQAgESETIAwhCyASIRQMAQsgEiEUIBEhEwNAIBNBHSATQR1JGyETAkAgDEF8aiILIBRJDQAgE60hFUIAIQgDQCALIAs1AgAgFYYgCEL/////D4N8IhYgFkKAlOvcA4AiCEKAlOvcA359PgIAIAtBfGoiCyAUTw0ACyAWQoCU69wDVA0AIBRBfGoiFCAIPgIACwJAA0AgDCILIBRNDQEgC0F8aiIMKAIARQ0ACwsgBiAGKAIsIBNrIhM2AiwgCyEMIBNBAEoNAAsLAkAgE0F/Sg0AIBBBGWpBCW5BAWohFyAPQeYARiEYA0BBACATayIMQQkgDEEJSRshDQJAAkAgFCALSQ0AIBQoAgBFQQJ0IQwMAQtBgJTr3AMgDXYhGUF/IA10QX9zIRpBACETIBQhDANAIAwgDCgCACIDIA12IBNqNgIAIAMgGnEgGWwhEyAMQQRqIgwgC0kNAAsgFCgCAEVBAnQhDCATRQ0AIAsgEzYCACALQQRqIQsLIAYgBigCLCANaiITNgIsIBIgFCAMaiIUIBgbIgwgF0ECdGogCyALIAxrQQJ1IBdKGyELIBNBAEgNAAsLQQAhEwJAIBQgC08NACASIBRrQQJ1QQlsIRNBCiEMIBQoAgAiA0EKSQ0AA0AgE0EBaiETIAMgDEEKbCIMTw0ACwsCQCAQQQAgEyAPQeYARhtrIBBBAEcgD0HnAEZxayIMIAsgEmtBAnVBCWxBd2pODQAgBkEwakGEYEGkYiARQQBIG2ogDEGAyABqIgNBCW0iGUECdGohDUEKIQwCQCADIBlBCWxrIgNBB0oNAANAIAxBCmwhDCADQQFqIgNBCEcNAAsLIA1BBGohGgJAAkAgDSgCACIDIAMgDG4iFyAMbGsiGQ0AIBogC0YNAQsCQAJAIBdBAXENAEQAAAAAAABAQyEBIAxBgJTr3ANHDQEgDSAUTQ0BIA1BfGotAABBAXFFDQELRAEAAAAAAEBDIQELRAAAAAAAAOA/RAAAAAAAAPA/RAAAAAAAAPg/IBogC0YbRAAAAAAAAPg/IBkgDEEBdiIaRhsgGSAaSRshGwJAIAcNACAKLQAAQS1HDQAgG5ohGyABmiEBCyANIAMgGWsiAzYCACABIBugIAFhDQAgDSADIAxqIgw2AgACQCAMQYCU69wDSQ0AA0AgDUEANgIAAkAgDUF8aiINIBRPDQAgFEF8aiIUQQA2AgALIA0gDSgCAEEBaiIMNgIAIAxB/5Pr3ANLDQALCyASIBRrQQJ1QQlsIRNBCiEMIBQoAgAiA0EKSQ0AA0AgE0EBaiETIAMgDEEKbCIMTw0ACwsgDUEEaiIMIAsgCyAMSxshCwsCQANAIAsiDCAUTSIDDQEgDEF8aiILKAIARQ0ACwsCQAJAIA9B5wBGDQAgBEEIcSEZDAELIBNBf3NBfyAQQQEgEBsiCyATSiATQXtKcSINGyALaiEQQX9BfiANGyAFaiEFIARBCHEiGQ0AQXchCwJAIAMNACAMQXxqKAIAIg1FDQBBCiEDQQAhCyANQQpwDQADQCALIhlBAWohCyANIANBCmwiA3BFDQALIBlBf3MhCwsgDCASa0ECdUEJbCEDAkAgBUFfcUHGAEcNAEEAIRkgECADIAtqQXdqIgtBACALQQBKGyILIBAgC0gbIRAMAQtBACEZIBAgEyADaiALakF3aiILQQAgC0EAShsiCyAQIAtIGyEQC0F/IQ0gEEH9////B0H+////ByAQIBlyIhobSg0BIBAgGkEAR2pBAWohAwJAAkAgBUFfcSIYQcYARw0AIBMgA0H/////B3NKDQMgE0EAIBNBAEobIQsMAQsCQCAOIBMgE0EfdSILcyALa60gDhDdgoCAACILa0EBSg0AA0AgC0F/aiILQTA6AAAgDiALa0ECSA0ACwsgC0F+aiIXIAU6AABBfyENIAtBf2pBLUErIBNBAEgbOgAAIA4gF2siCyADQf////8Hc0oNAgtBfyENIAsgA2oiCyAJQf////8Hc0oNASAAQSAgAiALIAlqIgUgBBDegoCAACAAIAogCRDYgoCAACAAQTAgAiAFIARBgIAEcxDegoCAAAJAAkACQAJAIBhBxgBHDQAgBkEQakEJciETIBIgFCAUIBJLGyIDIRQDQCAUNQIAIBMQ3YKAgAAhCwJAAkAgFCADRg0AIAsgBkEQak0NAQNAIAtBf2oiC0EwOgAAIAsgBkEQaksNAAwCCwsgCyATRw0AIAtBf2oiC0EwOgAACyAAIAsgEyALaxDYgoCAACAUQQRqIhQgEk0NAAsCQCAaRQ0AIABBvpeEgABBARDYgoCAAAsgFCAMTw0BIBBBAUgNAQNAAkAgFDUCACATEN2CgIAAIgsgBkEQak0NAANAIAtBf2oiC0EwOgAAIAsgBkEQaksNAAsLIAAgCyAQQQkgEEEJSBsQ2IKAgAAgEEF3aiELIBRBBGoiFCAMTw0DIBBBCUohAyALIRAgAw0ADAMLCwJAIBBBAEgNACAMIBRBBGogDCAUSxshDSAGQRBqQQlyIRMgFCEMA0ACQCAMNQIAIBMQ3YKAgAAiCyATRw0AIAtBf2oiC0EwOgAACwJAAkAgDCAURg0AIAsgBkEQak0NAQNAIAtBf2oiC0EwOgAAIAsgBkEQaksNAAwCCwsgACALQQEQ2IKAgAAgC0EBaiELIBAgGXJFDQAgAEG+l4SAAEEBENiCgIAACyAAIAsgEyALayIDIBAgECADShsQ2IKAgAAgECADayEQIAxBBGoiDCANTw0BIBBBf0oNAAsLIABBMCAQQRJqQRJBABDegoCAACAAIBcgDiAXaxDYgoCAAAwCCyAQIQsLIABBMCALQQlqQQlBABDegoCAAAsgAEEgIAIgBSAEQYDAAHMQ3oKAgAAgAiAFIAIgBUobIQ0MAQsgCiAFQRp0QR91QQlxaiEXAkAgA0ELSw0AQQwgA2shC0QAAAAAAAAwQCEbA0AgG0QAAAAAAAAwQKIhGyALQX9qIgsNAAsCQCAXLQAAQS1HDQAgGyABmiAboaCaIQEMAQsgASAboCAboSEBCwJAIAYoAiwiDCAMQR91IgtzIAtrrSAOEN2CgIAAIgsgDkcNACALQX9qIgtBMDoAACAGKAIsIQwLIAlBAnIhGSAFQSBxIRQgC0F+aiIaIAVBD2o6AAAgC0F/akEtQSsgDEEASBs6AAAgA0EBSCAEQQhxRXEhEyAGQRBqIQwDQCAMIgsgAfwCIgxBwMaEgABqLQAAIBRyOgAAIAEgDLehRAAAAAAAADBAoiEBAkAgC0EBaiIMIAZBEGprQQFHDQAgAUQAAAAAAAAAAGEgE3ENACALQS46AAEgC0ECaiEMCyABRAAAAAAAAAAAYg0AC0F/IQ0gA0H9////ByAZIA4gGmsiFGoiE2tKDQAgAEEgIAIgEyADQQJqIAwgBkEQamsiCyALQX5qIANIGyALIAMbIgNqIgwgBBDegoCAACAAIBcgGRDYgoCAACAAQTAgAiAMIARBgIAEcxDegoCAACAAIAZBEGogCxDYgoCAACAAQTAgAyALa0EAQQAQ3oKAgAAgACAaIBQQ2IKAgAAgAEEgIAIgDCAEQYDAAHMQ3oKAgAAgAiAMIAIgDEobIQ0LIAZBsARqJICAgIAAIA0LLgEBfyABIAEoAgBBB2pBeHEiAkEQajYCACAAIAIpAwAgAikDCBCCg4CAADkDAAsFACAAvQujAQECfyOAgICAAEGgAWsiBCSAgICAACAEIAAgBEGeAWogARsiADYClAEgBEEAIAFBf2oiBSAFIAFLGzYCmAECQEGQAUUNACAEQQBBkAH8CwALIARBfzYCTCAEQZSAgIAANgIkIARBfzYCUCAEIARBnwFqNgIsIAQgBEGUAWo2AlQgAEEAOgAAIAQgAiADEN+CgIAAIQEgBEGgAWokgICAgAAgAQu2AQEFfyAAKAJUIgMoAgAhBAJAIAMoAgQiBSAAKAIUIAAoAhwiBmsiByAFIAdJGyIHRQ0AIAQgBiAHEIiCgIAAGiADIAMoAgAgB2oiBDYCACADIAMoAgQgB2siBTYCBAsCQCAFIAIgBSACSRsiBUUNACAEIAEgBRCIgoCAABogAyADKAIAIAVqIgQ2AgAgAyADKAIEIAVrNgIECyAEQQA6AAAgACAAKAIsIgM2AhwgACADNgIUIAILGQACQCAADQBBAA8LEO+BgIAAIAA2AgBBfwssAQF+IABBADYCDCAAIAFCgJTr3AOAIgI3AwAgACABIAJCgJTr3AN+fT4CCAusAgEBf0EBIQMCQAJAIABFDQAgAUH/AE0NAQJAAkAQpoKAgAAoAmAoAgANACABQYB/cUGAvwNGDQMQ74GAgABBGTYCAAwBCwJAIAFB/w9LDQAgACABQT9xQYABcjoAASAAIAFBBnZBwAFyOgAAQQIPCwJAAkAgAUGAsANJDQAgAUGAQHFBgMADRw0BCyAAIAFBP3FBgAFyOgACIAAgAUEMdkHgAXI6AAAgACABQQZ2QT9xQYABcjoAAUEDDwsCQCABQYCAfGpB//8/Sw0AIAAgAUE/cUGAAXI6AAMgACABQRJ2QfABcjoAACAAIAFBBnZBP3FBgAFyOgACIAAgAUEMdkE/cUGAAXI6AAFBBA8LEO+BgIAAQRk2AgALQX8hAwsgAw8LIAAgAToAAEEBCxgAAkAgAA0AQQAPCyAAIAFBABDngoCAAAsJABC1gICAAAALkCcBDH8jgICAgABBEGsiASSAgICAAAJAAkACQAJAAkAgAEH0AUsNAAJAQQAoAojYhIAAIgJBECAAQQtqQfgDcSAAQQtJGyIDQQN2IgR2IgBBA3FFDQACQAJAIABBf3NBAXEgBGoiA0EDdCIAQbDYhIAAaiIFIABBuNiEgABqKAIAIgQoAggiAEcNAEEAIAJBfiADd3E2AojYhIAADAELIABBACgCmNiEgABJDQQgACgCDCAERw0EIAAgBTYCDCAFIAA2AggLIARBCGohACAEIANBA3QiA0EDcjYCBCAEIANqIgQgBCgCBEEBcjYCBAwFCyADQQAoApDYhIAAIgZNDQECQCAARQ0AAkACQCAAIAR0QQIgBHQiAEEAIABrcnFoIgVBA3QiAEGw2ISAAGoiByAAQbjYhIAAaigCACIAKAIIIgRHDQBBACACQX4gBXdxIgI2AojYhIAADAELIARBACgCmNiEgABJDQQgBCgCDCAARw0EIAQgBzYCDCAHIAQ2AggLIAAgA0EDcjYCBCAAIANqIgcgBUEDdCIEIANrIgNBAXI2AgQgACAEaiADNgIAAkAgBkUNACAGQXhxQbDYhIAAaiEFQQAoApzYhIAAIQQCQAJAIAJBASAGQQN2dCIIcQ0AQQAgAiAIcjYCiNiEgAAgBSEIDAELIAUoAggiCEEAKAKY2ISAAEkNBQsgBSAENgIIIAggBDYCDCAEIAU2AgwgBCAINgIICyAAQQhqIQBBACAHNgKc2ISAAEEAIAM2ApDYhIAADAULQQAoAozYhIAAIglFDQEgCWhBAnRBuNqEgABqKAIAIgcoAgRBeHEgA2shBCAHIQUCQANAAkAgBSgCECIADQAgBSgCFCIARQ0CCyAAKAIEQXhxIANrIgUgBCAFIARJIgUbIQQgACAHIAUbIQcgACEFDAALCyAHQQAoApjYhIAAIgpJDQIgBygCGCELAkACQCAHKAIMIgAgB0YNACAHKAIIIgUgCkkNBCAFKAIMIAdHDQQgACgCCCAHRw0EIAUgADYCDCAAIAU2AggMAQsCQAJAAkAgBygCFCIFRQ0AIAdBFGohCAwBCyAHKAIQIgVFDQEgB0EQaiEICwNAIAghDCAFIgBBFGohCCAAKAIUIgUNACAAQRBqIQggACgCECIFDQALIAwgCkkNBCAMQQA2AgAMAQtBACEACwJAIAtFDQACQAJAIAcgBygCHCIIQQJ0QbjahIAAaiIFKAIARw0AIAUgADYCACAADQFBACAJQX4gCHdxNgKM2ISAAAwCCyALIApJDQQCQAJAIAsoAhAgB0cNACALIAA2AhAMAQsgCyAANgIUCyAARQ0BCyAAIApJDQMgACALNgIYAkAgBygCECIFRQ0AIAUgCkkNBCAAIAU2AhAgBSAANgIYCyAHKAIUIgVFDQAgBSAKSQ0DIAAgBTYCFCAFIAA2AhgLAkACQCAEQQ9LDQAgByAEIANqIgBBA3I2AgQgByAAaiIAIAAoAgRBAXI2AgQMAQsgByADQQNyNgIEIAcgA2oiAyAEQQFyNgIEIAMgBGogBDYCAAJAIAZFDQAgBkF4cUGw2ISAAGohBUEAKAKc2ISAACEAAkACQEEBIAZBA3Z0IgggAnENAEEAIAggAnI2AojYhIAAIAUhCAwBCyAFKAIIIgggCkkNBQsgBSAANgIIIAggADYCDCAAIAU2AgwgACAINgIIC0EAIAM2ApzYhIAAQQAgBDYCkNiEgAALIAdBCGohAAwEC0F/IQMgAEG/f0sNACAAQQtqIgRBeHEhA0EAKAKM2ISAACILRQ0AQR8hBgJAIABB9P//B0sNACADQSYgBEEIdmciAGt2QQFxIABBAXRrQT5qIQYLQQAgA2shBAJAAkACQAJAIAZBAnRBuNqEgABqKAIAIgUNAEEAIQBBACEIDAELQQAhACADQQBBGSAGQQF2ayAGQR9GG3QhB0EAIQgDQAJAIAUoAgRBeHEgA2siAiAETw0AIAIhBCAFIQggAg0AQQAhBCAFIQggBSEADAMLIAAgBSgCFCICIAIgBSAHQR12QQRxaigCECIMRhsgACACGyEAIAdBAXQhByAMIQUgDA0ACwsCQCAAIAhyDQBBACEIQQIgBnQiAEEAIABrciALcSIARQ0DIABoQQJ0QbjahIAAaigCACEACyAARQ0BCwNAIAAoAgRBeHEgA2siAiAESSEHAkAgACgCECIFDQAgACgCFCEFCyACIAQgBxshBCAAIAggBxshCCAFIQAgBQ0ACwsgCEUNACAEQQAoApDYhIAAIANrTw0AIAhBACgCmNiEgAAiDEkNASAIKAIYIQYCQAJAIAgoAgwiACAIRg0AIAgoAggiBSAMSQ0DIAUoAgwgCEcNAyAAKAIIIAhHDQMgBSAANgIMIAAgBTYCCAwBCwJAAkACQCAIKAIUIgVFDQAgCEEUaiEHDAELIAgoAhAiBUUNASAIQRBqIQcLA0AgByECIAUiAEEUaiEHIAAoAhQiBQ0AIABBEGohByAAKAIQIgUNAAsgAiAMSQ0DIAJBADYCAAwBC0EAIQALAkAgBkUNAAJAAkAgCCAIKAIcIgdBAnRBuNqEgABqIgUoAgBHDQAgBSAANgIAIAANAUEAIAtBfiAHd3EiCzYCjNiEgAAMAgsgBiAMSQ0DAkACQCAGKAIQIAhHDQAgBiAANgIQDAELIAYgADYCFAsgAEUNAQsgACAMSQ0CIAAgBjYCGAJAIAgoAhAiBUUNACAFIAxJDQMgACAFNgIQIAUgADYCGAsgCCgCFCIFRQ0AIAUgDEkNAiAAIAU2AhQgBSAANgIYCwJAAkAgBEEPSw0AIAggBCADaiIAQQNyNgIEIAggAGoiACAAKAIEQQFyNgIEDAELIAggA0EDcjYCBCAIIANqIgcgBEEBcjYCBCAHIARqIAQ2AgACQCAEQf8BSw0AIARBeHFBsNiEgABqIQACQAJAQQAoAojYhIAAIgNBASAEQQN2dCIEcQ0AQQAgAyAEcjYCiNiEgAAgACEEDAELIAAoAggiBCAMSQ0ECyAAIAc2AgggBCAHNgIMIAcgADYCDCAHIAQ2AggMAQtBHyEAAkAgBEH///8HSw0AIARBJiAEQQh2ZyIAa3ZBAXEgAEEBdGtBPmohAAsgByAANgIcIAdCADcCECAAQQJ0QbjahIAAaiEDAkACQAJAIAtBASAAdCIFcQ0AQQAgCyAFcjYCjNiEgAAgAyAHNgIAIAcgAzYCGAwBCyAEQQBBGSAAQQF2ayAAQR9GG3QhACADKAIAIQUDQCAFIgMoAgRBeHEgBEYNAiAAQR12IQUgAEEBdCEAIAMgBUEEcWoiAigCECIFDQALIAJBEGoiACAMSQ0EIAAgBzYCACAHIAM2AhgLIAcgBzYCDCAHIAc2AggMAQsgAyAMSQ0CIAMoAggiACAMSQ0CIAAgBzYCDCADIAc2AgggB0EANgIYIAcgAzYCDCAHIAA2AggLIAhBCGohAAwDCwJAQQAoApDYhIAAIgAgA0kNAEEAKAKc2ISAACEEAkACQCAAIANrIgVBEEkNACAEIANqIgcgBUEBcjYCBCAEIABqIAU2AgAgBCADQQNyNgIEDAELIAQgAEEDcjYCBCAEIABqIgAgACgCBEEBcjYCBEEAIQdBACEFC0EAIAU2ApDYhIAAQQAgBzYCnNiEgAAgBEEIaiEADAMLAkBBACgClNiEgAAiByADTQ0AQQAgByADayIENgKU2ISAAEEAQQAoAqDYhIAAIgAgA2oiBTYCoNiEgAAgBSAEQQFyNgIEIAAgA0EDcjYCBCAAQQhqIQAMAwsCQAJAQQAoAuDbhIAARQ0AQQAoAujbhIAAIQQMAQtBAEJ/NwLs24SAAEEAQoCggICAgAQ3AuTbhIAAQQAgAUEMakFwcUHYqtWqBXM2AuDbhIAAQQBBADYC9NuEgABBAEEANgLE24SAAEGAICEEC0EAIQAgBCADQS9qIgZqIgJBACAEayIMcSIIIANNDQJBACEAAkBBACgCwNuEgAAiBEUNAEEAKAK424SAACIFIAhqIgsgBU0NAyALIARLDQMLAkACQAJAQQAtAMTbhIAAQQRxDQACQAJAAkACQAJAQQAoAqDYhIAAIgRFDQBByNuEgAAhAANAAkAgBCAAKAIAIgVJDQAgBCAFIAAoAgRqSQ0DCyAAKAIIIgANAAsLQQAQ8oKAgAAiB0F/Rg0DIAghAgJAQQAoAuTbhIAAIgBBf2oiBCAHcUUNACAIIAdrIAQgB2pBACAAa3FqIQILIAIgA00NAwJAQQAoAsDbhIAAIgBFDQBBACgCuNuEgAAiBCACaiIFIARNDQQgBSAASw0ECyACEPKCgIAAIgAgB0cNAQwFCyACIAdrIAxxIgIQ8oKAgAAiByAAKAIAIAAoAgRqRg0BIAchAAsgAEF/Rg0BAkAgAiADQTBqSQ0AIAAhBwwECyAGIAJrQQAoAujbhIAAIgRqQQAgBGtxIgQQ8oKAgABBf0YNASAEIAJqIQIgACEHDAMLIAdBf0cNAgtBAEEAKALE24SAAEEEcjYCxNuEgAALIAgQ8oKAgAAhB0EAEPKCgIAAIQAgB0F/Rg0BIABBf0YNASAHIABPDQEgACAHayICIANBKGpNDQELQQBBACgCuNuEgAAgAmoiADYCuNuEgAACQCAAQQAoArzbhIAATQ0AQQAgADYCvNuEgAALAkACQAJAAkBBACgCoNiEgAAiBEUNAEHI24SAACEAA0AgByAAKAIAIgUgACgCBCIIakYNAiAAKAIIIgANAAwDCwsCQAJAQQAoApjYhIAAIgBFDQAgByAATw0BC0EAIAc2ApjYhIAAC0EAIQBBACACNgLM24SAAEEAIAc2AsjbhIAAQQBBfzYCqNiEgABBAEEAKALg24SAADYCrNiEgABBAEEANgLU24SAAANAIABBA3QiBEG42ISAAGogBEGw2ISAAGoiBTYCACAEQbzYhIAAaiAFNgIAIABBAWoiAEEgRw0AC0EAIAJBWGoiAEF4IAdrQQdxIgRrIgU2ApTYhIAAQQAgByAEaiIENgKg2ISAACAEIAVBAXI2AgQgByAAakEoNgIEQQBBACgC8NuEgAA2AqTYhIAADAILIAQgB08NACAEIAVJDQAgACgCDEEIcQ0AIAAgCCACajYCBEEAIARBeCAEa0EHcSIAaiIFNgKg2ISAAEEAQQAoApTYhIAAIAJqIgcgAGsiADYClNiEgAAgBSAAQQFyNgIEIAQgB2pBKDYCBEEAQQAoAvDbhIAANgKk2ISAAAwBCwJAIAdBACgCmNiEgABPDQBBACAHNgKY2ISAAAsgByACaiEFQcjbhIAAIQACQAJAA0AgACgCACIIIAVGDQEgACgCCCIADQAMAgsLIAAtAAxBCHFFDQQLQcjbhIAAIQACQANAAkAgBCAAKAIAIgVJDQAgBCAFIAAoAgRqIgVJDQILIAAoAgghAAwACwtBACACQVhqIgBBeCAHa0EHcSIIayIMNgKU2ISAAEEAIAcgCGoiCDYCoNiEgAAgCCAMQQFyNgIEIAcgAGpBKDYCBEEAQQAoAvDbhIAANgKk2ISAACAEIAVBJyAFa0EHcWpBUWoiACAAIARBEGpJGyIIQRs2AgQgCEEQakEAKQLQ24SAADcCACAIQQApAsjbhIAANwIIQQAgCEEIajYC0NuEgABBACACNgLM24SAAEEAIAc2AsjbhIAAQQBBADYC1NuEgAAgCEEYaiEAA0AgAEEHNgIEIABBCGohByAAQQRqIQAgByAFSQ0ACyAIIARGDQAgCCAIKAIEQX5xNgIEIAQgCCAEayIHQQFyNgIEIAggBzYCAAJAAkAgB0H/AUsNACAHQXhxQbDYhIAAaiEAAkACQEEAKAKI2ISAACIFQQEgB0EDdnQiB3ENAEEAIAUgB3I2AojYhIAAIAAhBQwBCyAAKAIIIgVBACgCmNiEgABJDQULIAAgBDYCCCAFIAQ2AgxBDCEHQQghCAwBC0EfIQACQCAHQf///wdLDQAgB0EmIAdBCHZnIgBrdkEBcSAAQQF0a0E+aiEACyAEIAA2AhwgBEIANwIQIABBAnRBuNqEgABqIQUCQAJAAkBBACgCjNiEgAAiCEEBIAB0IgJxDQBBACAIIAJyNgKM2ISAACAFIAQ2AgAgBCAFNgIYDAELIAdBAEEZIABBAXZrIABBH0YbdCEAIAUoAgAhCANAIAgiBSgCBEF4cSAHRg0CIABBHXYhCCAAQQF0IQAgBSAIQQRxaiICKAIQIggNAAsgAkEQaiIAQQAoApjYhIAASQ0FIAAgBDYCACAEIAU2AhgLQQghB0EMIQggBCEFIAQhAAwBCyAFQQAoApjYhIAAIgdJDQMgBSgCCCIAIAdJDQMgACAENgIMIAUgBDYCCCAEIAA2AghBACEAQRghB0EMIQgLIAQgCGogBTYCACAEIAdqIAA2AgALQQAoApTYhIAAIgAgA00NAEEAIAAgA2siBDYClNiEgABBAEEAKAKg2ISAACIAIANqIgU2AqDYhIAAIAUgBEEBcjYCBCAAIANBA3I2AgQgAEEIaiEADAMLEO+BgIAAQTA2AgBBACEADAILEOmCgIAAAAsgACAHNgIAIAAgACgCBCACajYCBCAHIAggAxDrgoCAACEACyABQRBqJICAgIAAIAALhgoBB38gAEF4IABrQQdxaiIDIAJBA3I2AgQgAUF4IAFrQQdxaiIEIAMgAmoiBWshAAJAAkACQCAEQQAoAqDYhIAARw0AQQAgBTYCoNiEgABBAEEAKAKU2ISAACAAaiICNgKU2ISAACAFIAJBAXI2AgQMAQsCQCAEQQAoApzYhIAARw0AQQAgBTYCnNiEgABBAEEAKAKQ2ISAACAAaiICNgKQ2ISAACAFIAJBAXI2AgQgBSACaiACNgIADAELAkAgBCgCBCIGQQNxQQFHDQAgBCgCDCECAkACQCAGQf8BSw0AAkAgBCgCCCIBIAZBA3YiB0EDdEGw2ISAAGoiCEYNACABQQAoApjYhIAASQ0FIAEoAgwgBEcNBQsCQCACIAFHDQBBAEEAKAKI2ISAAEF+IAd3cTYCiNiEgAAMAgsCQCACIAhGDQAgAkEAKAKY2ISAAEkNBSACKAIIIARHDQULIAEgAjYCDCACIAE2AggMAQsgBCgCGCEJAkACQCACIARGDQAgBCgCCCIBQQAoApjYhIAASQ0FIAEoAgwgBEcNBSACKAIIIARHDQUgASACNgIMIAIgATYCCAwBCwJAAkACQCAEKAIUIgFFDQAgBEEUaiEIDAELIAQoAhAiAUUNASAEQRBqIQgLA0AgCCEHIAEiAkEUaiEIIAIoAhQiAQ0AIAJBEGohCCACKAIQIgENAAsgB0EAKAKY2ISAAEkNBSAHQQA2AgAMAQtBACECCyAJRQ0AAkACQCAEIAQoAhwiCEECdEG42oSAAGoiASgCAEcNACABIAI2AgAgAg0BQQBBACgCjNiEgABBfiAId3E2AozYhIAADAILIAlBACgCmNiEgABJDQQCQAJAIAkoAhAgBEcNACAJIAI2AhAMAQsgCSACNgIUCyACRQ0BCyACQQAoApjYhIAAIghJDQMgAiAJNgIYAkAgBCgCECIBRQ0AIAEgCEkNBCACIAE2AhAgASACNgIYCyAEKAIUIgFFDQAgASAISQ0DIAIgATYCFCABIAI2AhgLIAZBeHEiAiAAaiEAIAQgAmoiBCgCBCEGCyAEIAZBfnE2AgQgBSAAQQFyNgIEIAUgAGogADYCAAJAIABB/wFLDQAgAEF4cUGw2ISAAGohAgJAAkBBACgCiNiEgAAiAUEBIABBA3Z0IgBxDQBBACABIAByNgKI2ISAACACIQAMAQsgAigCCCIAQQAoApjYhIAASQ0DCyACIAU2AgggACAFNgIMIAUgAjYCDCAFIAA2AggMAQtBHyECAkAgAEH///8HSw0AIABBJiAAQQh2ZyICa3ZBAXEgAkEBdGtBPmohAgsgBSACNgIcIAVCADcCECACQQJ0QbjahIAAaiEBAkACQAJAQQAoAozYhIAAIghBASACdCIEcQ0AQQAgCCAEcjYCjNiEgAAgASAFNgIAIAUgATYCGAwBCyAAQQBBGSACQQF2ayACQR9GG3QhAiABKAIAIQgDQCAIIgEoAgRBeHEgAEYNAiACQR12IQggAkEBdCECIAEgCEEEcWoiBCgCECIIDQALIARBEGoiAkEAKAKY2ISAAEkNAyACIAU2AgAgBSABNgIYCyAFIAU2AgwgBSAFNgIIDAELIAFBACgCmNiEgAAiAEkNASABKAIIIgIgAEkNASACIAU2AgwgASAFNgIIIAVBADYCGCAFIAE2AgwgBSACNgIICyADQQhqDwsQ6YKAgAAAC70PAQp/AkACQCAARQ0AIABBeGoiAUEAKAKY2ISAACICSQ0BIABBfGooAgAiA0EDcUEBRg0BIAEgA0F4cSIAaiEEAkAgA0EBcQ0AIANBAnFFDQEgASABKAIAIgVrIgEgAkkNAiAFIABqIQACQCABQQAoApzYhIAARg0AIAEoAgwhAwJAIAVB/wFLDQACQCABKAIIIgYgBUEDdiIHQQN0QbDYhIAAaiIFRg0AIAYgAkkNBSAGKAIMIAFHDQULAkAgAyAGRw0AQQBBACgCiNiEgABBfiAHd3E2AojYhIAADAMLAkAgAyAFRg0AIAMgAkkNBSADKAIIIAFHDQULIAYgAzYCDCADIAY2AggMAgsgASgCGCEIAkACQCADIAFGDQAgASgCCCIFIAJJDQUgBSgCDCABRw0FIAMoAgggAUcNBSAFIAM2AgwgAyAFNgIIDAELAkACQAJAIAEoAhQiBUUNACABQRRqIQYMAQsgASgCECIFRQ0BIAFBEGohBgsDQCAGIQcgBSIDQRRqIQYgAygCFCIFDQAgA0EQaiEGIAMoAhAiBQ0ACyAHIAJJDQUgB0EANgIADAELQQAhAwsgCEUNAQJAAkAgASABKAIcIgZBAnRBuNqEgABqIgUoAgBHDQAgBSADNgIAIAMNAUEAQQAoAozYhIAAQX4gBndxNgKM2ISAAAwDCyAIIAJJDQQCQAJAIAgoAhAgAUcNACAIIAM2AhAMAQsgCCADNgIUCyADRQ0CCyADIAJJDQMgAyAINgIYAkAgASgCECIFRQ0AIAUgAkkNBCADIAU2AhAgBSADNgIYCyABKAIUIgVFDQEgBSACSQ0DIAMgBTYCFCAFIAM2AhgMAQsgBCgCBCIDQQNxQQNHDQBBACAANgKQ2ISAACAEIANBfnE2AgQgASAAQQFyNgIEIAQgADYCAA8LIAEgBE8NASAEKAIEIgdBAXFFDQECQAJAIAdBAnENAAJAIARBACgCoNiEgABHDQBBACABNgKg2ISAAEEAQQAoApTYhIAAIABqIgA2ApTYhIAAIAEgAEEBcjYCBCABQQAoApzYhIAARw0DQQBBADYCkNiEgABBAEEANgKc2ISAAA8LAkAgBEEAKAKc2ISAACIJRw0AQQAgATYCnNiEgABBAEEAKAKQ2ISAACAAaiIANgKQ2ISAACABIABBAXI2AgQgASAAaiAANgIADwsgBCgCDCEDAkACQCAHQf8BSw0AAkAgBCgCCCIFIAdBA3YiCEEDdEGw2ISAAGoiBkYNACAFIAJJDQYgBSgCDCAERw0GCwJAIAMgBUcNAEEAQQAoAojYhIAAQX4gCHdxNgKI2ISAAAwCCwJAIAMgBkYNACADIAJJDQYgAygCCCAERw0GCyAFIAM2AgwgAyAFNgIIDAELIAQoAhghCgJAAkAgAyAERg0AIAQoAggiBSACSQ0GIAUoAgwgBEcNBiADKAIIIARHDQYgBSADNgIMIAMgBTYCCAwBCwJAAkACQCAEKAIUIgVFDQAgBEEUaiEGDAELIAQoAhAiBUUNASAEQRBqIQYLA0AgBiEIIAUiA0EUaiEGIAMoAhQiBQ0AIANBEGohBiADKAIQIgUNAAsgCCACSQ0GIAhBADYCAAwBC0EAIQMLIApFDQACQAJAIAQgBCgCHCIGQQJ0QbjahIAAaiIFKAIARw0AIAUgAzYCACADDQFBAEEAKAKM2ISAAEF+IAZ3cTYCjNiEgAAMAgsgCiACSQ0FAkACQCAKKAIQIARHDQAgCiADNgIQDAELIAogAzYCFAsgA0UNAQsgAyACSQ0EIAMgCjYCGAJAIAQoAhAiBUUNACAFIAJJDQUgAyAFNgIQIAUgAzYCGAsgBCgCFCIFRQ0AIAUgAkkNBCADIAU2AhQgBSADNgIYCyABIAdBeHEgAGoiAEEBcjYCBCABIABqIAA2AgAgASAJRw0BQQAgADYCkNiEgAAPCyAEIAdBfnE2AgQgASAAQQFyNgIEIAEgAGogADYCAAsCQCAAQf8BSw0AIABBeHFBsNiEgABqIQMCQAJAQQAoAojYhIAAIgVBASAAQQN2dCIAcQ0AQQAgBSAAcjYCiNiEgAAgAyEADAELIAMoAggiACACSQ0DCyADIAE2AgggACABNgIMIAEgAzYCDCABIAA2AggPC0EfIQMCQCAAQf///wdLDQAgAEEmIABBCHZnIgNrdkEBcSADQQF0a0E+aiEDCyABIAM2AhwgAUIANwIQIANBAnRBuNqEgABqIQYCQAJAAkACQEEAKAKM2ISAACIFQQEgA3QiBHENAEEAIAUgBHI2AozYhIAAIAYgATYCAEEIIQBBGCEDDAELIABBAEEZIANBAXZrIANBH0YbdCEDIAYoAgAhBgNAIAYiBSgCBEF4cSAARg0CIANBHXYhBiADQQF0IQMgBSAGQQRxaiIEKAIQIgYNAAsgBEEQaiIAIAJJDQQgACABNgIAQQghAEEYIQMgBSEGCyABIQUgASEEDAELIAUgAkkNAiAFKAIIIgYgAkkNAiAGIAE2AgwgBSABNgIIQQAhBEEYIQBBCCEDCyABIANqIAY2AgAgASAFNgIMIAEgAGogBDYCAEEAQQAoAqjYhIAAQX9qIgFBfyABGzYCqNiEgAALDwsQ6YKAgAAAC54BAQJ/AkAgAA0AIAEQ6oKAgAAPCwJAIAFBQEkNABDvgYCAAEEwNgIAQQAPCwJAIABBeGpBECABQQtqQXhxIAFBC0kbEO6CgIAAIgJFDQAgAkEIag8LAkAgARDqgoCAACICDQBBAA8LIAIgAEF8QXggAEF8aigCACIDQQNxGyADQXhxaiIDIAEgAyABSRsQiIKAgAAaIAAQ7IKAgAAgAguRCQEJfwJAAkAgAEEAKAKY2ISAACICSQ0AIAAoAgQiA0EDcSIEQQFGDQAgA0F4cSIFRQ0AIAAgBWoiBigCBCIHQQFxRQ0AAkAgBA0AQQAhBCABQYACSQ0CAkAgBSABQQRqSQ0AIAAhBCAFIAFrQQAoAujbhIAAQQF0TQ0DC0EAIQQMAgsCQCAFIAFJDQACQCAFIAFrIgVBEEkNACAAIAEgA0EBcXJBAnI2AgQgACABaiIBIAVBA3I2AgQgBiAGKAIEQQFyNgIEIAEgBRDvgoCAAAsgAA8LQQAhBAJAIAZBACgCoNiEgABHDQBBACgClNiEgAAgBWoiBSABTQ0CIAAgASADQQFxckECcjYCBCAAIAFqIgMgBSABayIFQQFyNgIEQQAgBTYClNiEgABBACADNgKg2ISAACAADwsCQCAGQQAoApzYhIAARw0AQQAhBEEAKAKQ2ISAACAFaiIFIAFJDQICQAJAIAUgAWsiBEEQSQ0AIAAgASADQQFxckECcjYCBCAAIAFqIgEgBEEBcjYCBCAAIAVqIgUgBDYCACAFIAUoAgRBfnE2AgQMAQsgACADQQFxIAVyQQJyNgIEIAAgBWoiBSAFKAIEQQFyNgIEQQAhBEEAIQELQQAgATYCnNiEgABBACAENgKQ2ISAACAADwtBACEEIAdBAnENASAHQXhxIAVqIgggAUkNASAGKAIMIQUCQAJAIAdB/wFLDQACQCAGKAIIIgQgB0EDdiIJQQN0QbDYhIAAaiIHRg0AIAQgAkkNAyAEKAIMIAZHDQMLAkAgBSAERw0AQQBBACgCiNiEgABBfiAJd3E2AojYhIAADAILAkAgBSAHRg0AIAUgAkkNAyAFKAIIIAZHDQMLIAQgBTYCDCAFIAQ2AggMAQsgBigCGCEKAkACQCAFIAZGDQAgBigCCCIEIAJJDQMgBCgCDCAGRw0DIAUoAgggBkcNAyAEIAU2AgwgBSAENgIIDAELAkACQAJAIAYoAhQiBEUNACAGQRRqIQcMAQsgBigCECIERQ0BIAZBEGohBwsDQCAHIQkgBCIFQRRqIQcgBSgCFCIEDQAgBUEQaiEHIAUoAhAiBA0ACyAJIAJJDQMgCUEANgIADAELQQAhBQsgCkUNAAJAAkAgBiAGKAIcIgdBAnRBuNqEgABqIgQoAgBHDQAgBCAFNgIAIAUNAUEAQQAoAozYhIAAQX4gB3dxNgKM2ISAAAwCCyAKIAJJDQICQAJAIAooAhAgBkcNACAKIAU2AhAMAQsgCiAFNgIUCyAFRQ0BCyAFIAJJDQEgBSAKNgIYAkAgBigCECIERQ0AIAQgAkkNAiAFIAQ2AhAgBCAFNgIYCyAGKAIUIgRFDQAgBCACSQ0BIAUgBDYCFCAEIAU2AhgLAkAgCCABayIFQQ9LDQAgACADQQFxIAhyQQJyNgIEIAAgCGoiBSAFKAIEQQFyNgIEIAAPCyAAIAEgA0EBcXJBAnI2AgQgACABaiIBIAVBA3I2AgQgACAIaiIDIAMoAgRBAXI2AgQgASAFEO+CgIAAIAAPCxDpgoCAAAALIAQL8Q4BCX8gACABaiECAkACQAJAAkAgACgCBCIDQQFxRQ0AQQAoApjYhIAAIQQMAQsgA0ECcUUNASAAIAAoAgAiBWsiAEEAKAKY2ISAACIESQ0CIAUgAWohAQJAIABBACgCnNiEgABGDQAgACgCDCEDAkAgBUH/AUsNAAJAIAAoAggiBiAFQQN2IgdBA3RBsNiEgABqIgVGDQAgBiAESQ0FIAYoAgwgAEcNBQsCQCADIAZHDQBBAEEAKAKI2ISAAEF+IAd3cTYCiNiEgAAMAwsCQCADIAVGDQAgAyAESQ0FIAMoAgggAEcNBQsgBiADNgIMIAMgBjYCCAwCCyAAKAIYIQgCQAJAIAMgAEYNACAAKAIIIgUgBEkNBSAFKAIMIABHDQUgAygCCCAARw0FIAUgAzYCDCADIAU2AggMAQsCQAJAAkAgACgCFCIFRQ0AIABBFGohBgwBCyAAKAIQIgVFDQEgAEEQaiEGCwNAIAYhByAFIgNBFGohBiADKAIUIgUNACADQRBqIQYgAygCECIFDQALIAcgBEkNBSAHQQA2AgAMAQtBACEDCyAIRQ0BAkACQCAAIAAoAhwiBkECdEG42oSAAGoiBSgCAEcNACAFIAM2AgAgAw0BQQBBACgCjNiEgABBfiAGd3E2AozYhIAADAMLIAggBEkNBAJAAkAgCCgCECAARw0AIAggAzYCEAwBCyAIIAM2AhQLIANFDQILIAMgBEkNAyADIAg2AhgCQCAAKAIQIgVFDQAgBSAESQ0EIAMgBTYCECAFIAM2AhgLIAAoAhQiBUUNASAFIARJDQMgAyAFNgIUIAUgAzYCGAwBCyACKAIEIgNBA3FBA0cNAEEAIAE2ApDYhIAAIAIgA0F+cTYCBCAAIAFBAXI2AgQgAiABNgIADwsgAiAESQ0BAkACQCACKAIEIghBAnENAAJAIAJBACgCoNiEgABHDQBBACAANgKg2ISAAEEAQQAoApTYhIAAIAFqIgE2ApTYhIAAIAAgAUEBcjYCBCAAQQAoApzYhIAARw0DQQBBADYCkNiEgABBAEEANgKc2ISAAA8LAkAgAkEAKAKc2ISAACIJRw0AQQAgADYCnNiEgABBAEEAKAKQ2ISAACABaiIBNgKQ2ISAACAAIAFBAXI2AgQgACABaiABNgIADwsgAigCDCEDAkACQCAIQf8BSw0AAkAgAigCCCIFIAhBA3YiB0EDdEGw2ISAAGoiBkYNACAFIARJDQYgBSgCDCACRw0GCwJAIAMgBUcNAEEAQQAoAojYhIAAQX4gB3dxNgKI2ISAAAwCCwJAIAMgBkYNACADIARJDQYgAygCCCACRw0GCyAFIAM2AgwgAyAFNgIIDAELIAIoAhghCgJAAkAgAyACRg0AIAIoAggiBSAESQ0GIAUoAgwgAkcNBiADKAIIIAJHDQYgBSADNgIMIAMgBTYCCAwBCwJAAkACQCACKAIUIgVFDQAgAkEUaiEGDAELIAIoAhAiBUUNASACQRBqIQYLA0AgBiEHIAUiA0EUaiEGIAMoAhQiBQ0AIANBEGohBiADKAIQIgUNAAsgByAESQ0GIAdBADYCAAwBC0EAIQMLIApFDQACQAJAIAIgAigCHCIGQQJ0QbjahIAAaiIFKAIARw0AIAUgAzYCACADDQFBAEEAKAKM2ISAAEF+IAZ3cTYCjNiEgAAMAgsgCiAESQ0FAkACQCAKKAIQIAJHDQAgCiADNgIQDAELIAogAzYCFAsgA0UNAQsgAyAESQ0EIAMgCjYCGAJAIAIoAhAiBUUNACAFIARJDQUgAyAFNgIQIAUgAzYCGAsgAigCFCIFRQ0AIAUgBEkNBCADIAU2AhQgBSADNgIYCyAAIAhBeHEgAWoiAUEBcjYCBCAAIAFqIAE2AgAgACAJRw0BQQAgATYCkNiEgAAPCyACIAhBfnE2AgQgACABQQFyNgIEIAAgAWogATYCAAsCQCABQf8BSw0AIAFBeHFBsNiEgABqIQMCQAJAQQAoAojYhIAAIgVBASABQQN2dCIBcQ0AQQAgBSABcjYCiNiEgAAgAyEBDAELIAMoAggiASAESQ0DCyADIAA2AgggASAANgIMIAAgAzYCDCAAIAE2AggPC0EfIQMCQCABQf///wdLDQAgAUEmIAFBCHZnIgNrdkEBcSADQQF0a0E+aiEDCyAAIAM2AhwgAEIANwIQIANBAnRBuNqEgABqIQUCQAJAAkBBACgCjNiEgAAiBkEBIAN0IgJxDQBBACAGIAJyNgKM2ISAACAFIAA2AgAgACAFNgIYDAELIAFBAEEZIANBAXZrIANBH0YbdCEDIAUoAgAhBgNAIAYiBSgCBEF4cSABRg0CIANBHXYhBiADQQF0IQMgBSAGQQRxaiICKAIQIgYNAAsgAkEQaiIBIARJDQMgASAANgIAIAAgBTYCGAsgACAANgIMIAAgADYCCA8LIAUgBEkNASAFKAIIIgEgBEkNASABIAA2AgwgBSAANgIIIABBADYCGCAAIAU2AgwgACABNgIICw8LEOmCgIAAAAtrAgF/AX4CQAJAIAANAEEAIQIMAQsgAK0gAa1+IgOnIQIgASAAckGAgARJDQBBfyACIANCIIinQQBHGyECCwJAIAIQ6oKAgAAiAEUNACAAQXxqLQAAQQNxRQ0AIABBACACEP6BgIAAGgsgAAsHAD8AQRB0C2EBAn9BACgCvMqEgAAiASAAQQdqQXhxIgJqIQACQAJAAkAgAkUNACAAIAFNDQELIAAQ8YKAgABNDQEgABC2gICAAA0BCxDvgYCAAEEwNgIAQX8PC0EAIAA2ArzKhIAAIAEL+goHAX8BfgF/An4BfwF+AX8jgICAgABB8ABrIgUkgICAgAAgBEL///////////8AgyEGAkACQAJAIAFQIgcgAkL///////////8AgyIIQoCAgICAgMCAgH98QoCAgICAgMCAgH9UIAhQGw0AIANCAFIgBkKAgICAgIDAgIB/fCIJQoCAgICAgMCAgH9WIAlCgICAgICAwICAf1EbDQELAkAgByAIQoCAgICAgMD//wBUIAhCgICAgICAwP//AFEbDQAgAkKAgICAgIAghCEEIAEhAwwCCwJAIANQIAZCgICAgICAwP//AFQgBkKAgICAgIDA//8AURsNACAEQoCAgICAgCCEIQQMAgsCQCABIAhCgICAgICAwP//AIWEQgBSDQBCgICAgICA4P//ACACIAMgAYUgBCAChUKAgICAgICAgIB/hYRQIgcbIQRCACABIAcbIQMMAgsgAyAGQoCAgICAgMD//wCFhFANAQJAIAEgCIRCAFINACADIAaEQgBSDQIgAyABgyEDIAQgAoMhBAwCCyADIAaEUEUNACABIQMgAiEEDAELIAMgASADIAFWIAYgCFYgBiAIURsiChshBiAEIAIgChsiCUL///////8/gyEIIAIgBCAKGyILQjCIp0H//wFxIQwCQCAJQjCIp0H//wFxIgcNACAFQeAAaiAGIAggBiAIIAhQIgcbeSAHQQZ0rXynIgdBcWoQ9IKAgABBECAHayEHIAUpA2ghCCAFKQNgIQYLIAEgAyAKGyEDIAtC////////P4MhAQJAIAwNACAFQdAAaiADIAEgAyABIAFQIgobeSAKQQZ0rXynIgpBcWoQ9IKAgABBECAKayEMIAUpA1ghASAFKQNQIQMLIAFCA4YgA0I9iIRCgICAgICAgASEIQEgCEIDhiAGQj2IhCELIANCA4YhCCAEIAKFIQMCQCAHIAxGDQACQCAHIAxrIgpB/wBNDQBCACEBQgEhCAwBCyAFQcAAaiAIIAFBgAEgCmsQ9IKAgAAgBUEwaiAIIAEgChD+goCAACAFKQMwIAUpA0AgBSkDSIRCAFKthCEIIAUpAzghAQsgC0KAgICAgICABIQhCyAGQgOGIQYCQAJAIANCf1UNAEIAIQNCACEEIAYgCIUgCyABhYRQDQIgBiAIfSECIAsgAX0gBiAIVK19IgRC/////////wNWDQEgBUEgaiACIAQgAiAEIARQIgobeSAKQQZ0rXynQXRqIgoQ9IKAgAAgByAKayEHIAUpAyghBCAFKQMgIQIMAQsgASALfCAIIAZ8IgIgCFStfCIEQoCAgICAgIAIg1ANACACQgGIIARCP4aEIAhCAYOEIQIgB0EBaiEHIARCAYghBAsgCUKAgICAgICAgIB/gyEIAkAgB0H//wFIDQAgCEKAgICAgIDA//8AhCEEQgAhAwwBC0EAIQoCQAJAIAdBAEwNACAHIQoMAQsgBUEQaiACIAQgB0H/AGoQ9IKAgAAgBSACIARBASAHaxD+goCAACAFKQMAIAUpAxAgBSkDGIRCAFKthCECIAUpAwghBAsgAkIDiCAEQj2GhCEDIAqtQjCGIARCA4hC////////P4OEIAiEIQQgAqdBB3EhBwJAAkACQAJAAkAQ/IKAgAAOAwABAgMLAkAgB0EERg0AIAQgAyAHQQRLrXwiCCADVK18IQQgCCEDDAMLIAQgAyADQgGDfCIIIANUrXwhBCAIIQMMAwsgBCADIAhCAFIgB0EAR3GtfCIIIANUrXwhBCAIIQMMAQsgBCADIAhQIAdBAEdxrXwiCCADVK18IQQgCCEDCyAHRQ0BCxD9goCAABoLIAAgAzcDACAAIAQ3AwggBUHwAGokgICAgAALUwEBfgJAAkAgA0HAAHFFDQAgASADQUBqrYYhAkIAIQEMAQsgA0UNACABQcAAIANrrYggAiADrSIEhoQhAiABIASGIQELIAAgATcDACAAIAI3AwgL5gECAX8CfkEBIQQCQCAAQgBSIAFC////////////AIMiBUKAgICAgIDA//8AViAFQoCAgICAgMD//wBRGw0AIAJCAFIgA0L///////////8AgyIGQoCAgICAgMD//wBWIAZCgICAgICAwP//AFEbDQACQCACIACEIAYgBYSEUEUNAEEADwsCQCADIAGDQgBTDQACQCAAIAJUIAEgA1MgASADURtFDQBBfw8LIAAgAoUgASADhYRCAFIPCwJAIAAgAlYgASADVSABIANRG0UNAEF/DwsgACAChSABIAOFhEIAUiEECyAEC9gBAgF/An5BfyEEAkAgAEIAUiABQv///////////wCDIgVCgICAgICAwP//AFYgBUKAgICAgIDA//8AURsNACACQgBSIANC////////////AIMiBkKAgICAgIDA//8AViAGQoCAgICAgMD//wBRGw0AAkAgAiAAhCAGIAWEhFBFDQBBAA8LAkAgAyABg0IAUw0AIAAgAlQgASADUyABIANRGw0BIAAgAoUgASADhYRCAFIPCyAAIAJWIAEgA1UgASADURsNACAAIAKFIAEgA4WEQgBSIQQLIAQLwRAGAX8DfgN/AX4Bfwt+I4CAgIAAQdACayIFJICAgIAAIARC////////P4MhBiACQv///////z+DIQcgBCAChUKAgICAgICAgIB/gyEIIARCMIinQf//AXEhCQJAAkACQCACQjCIp0H//wFxIgpBgYB+akGCgH5JDQBBACELIAlBgYB+akGBgH5LDQELAkAgAVAgAkL///////////8AgyIMQoCAgICAgMD//wBUIAxCgICAgICAwP//AFEbDQAgAkKAgICAgIAghCEIDAILAkAgA1AgBEL///////////8AgyICQoCAgICAgMD//wBUIAJCgICAgICAwP//AFEbDQAgBEKAgICAgIAghCEIIAMhAQwCCwJAIAEgDEKAgICAgIDA//8AhYRCAFINAAJAIAMgAkKAgICAgIDA//8AhYRQRQ0AQgAhAUKAgICAgIDg//8AIQgMAwsgCEKAgICAgIDA//8AhCEIQgAhAQwCCwJAIAMgAkKAgICAgIDA//8AhYRCAFINAEIAIQEMAgsCQCABIAyEQgBSDQBCgICAgICA4P//ACAIIAMgAoRQGyEIQgAhAQwCCwJAIAMgAoRCAFINACAIQoCAgICAgMD//wCEIQhCACEBDAILQQAhCwJAIAxC////////P1YNACAFQcACaiABIAcgASAHIAdQIgsbeSALQQZ0rXynIgtBcWoQ9IKAgABBECALayELIAUpA8gCIQcgBSkDwAIhAQsgAkL///////8/Vg0AIAVBsAJqIAMgBiADIAYgBlAiDRt5IA1BBnStfKciDUFxahD0goCAACANIAtqQXBqIQsgBSkDuAIhBiAFKQOwAiEDCyAFQaACaiADQjGIIAZCgICAgICAwACEIg5CD4aEIgJCAEKAgICAsOa8gvUAIAJ9IgRC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QQAhBEIAIQcMAQsgAiAErUIAIARnIgRB0QBqEPSCgIAAQYn/ACAEayEEIAIpAwhCgICAgICAwACFIQcgAikDACEICyAAIAg3AwAgACAErUIwhiADQR92rUI/hoQgB4Q3AwggAkEQaiSAgICAAAubAQMBfwJ+AX8jgICAgABBEGsiAiSAgICAAAJAAkAgAQ0AQgAhA0IAIQQMAQsgAiABIAFBH3UiBXMgBWsiBa1CACAFZyIFQdEAahD0goCAACACKQMIQoCAgICAgMAAhUGegAEgBWutQjCGfCABQYCAgIB4ca1CIIaEIQQgAikDACEDCyAAIAM3AwAgACAENwMIIAJBEGokgICAgAALgQECAX8CfiOAgICAAEEQayICJICAgIAAAkACQCABDQBCACEDQgAhBAwBCyACIAGtQgBB8AAgAWciAUEfc2sQ9IKAgAAgAikDCEKAgICAgIDAAIVBnoABIAFrrUIwhnwhBCACKQMAIQMLIAAgAzcDACAAIAQ3AwggAkEQaiSAgICAAAsEAEEACwQAQQALUwEBfgJAAkAgA0HAAHFFDQAgAiADQUBqrYghAUIAIQIMAQsgA0UNACACQcAAIANrrYYgASADrSIEiIQhASACIASIIQILIAAgATcDACAAIAI3AwgLowsGAX8EfgN/AX4Bfwp+I4CAgIAAQeAAayIFJICAgIAAIARC////////P4MhBiAEIAKFQoCAgICAgICAgH+DIQcgAkL///////8/gyIIQiCIIQkgBEIwiKdB//8BcSEKAkACQAJAIAJCMIinQf//AXEiC0GBgH5qQYKAfkkNAEEAIQwgCkGBgH5qQYGAfksNAQsCQCABUCACQv///////////wCDIg1CgICAgICAwP//AFQgDUKAgICAgIDA//8AURsNACACQoCAgICAgCCEIQcMAgsCQCADUCAEQv///////////wCDIgJCgICAgICAwP//AFQgAkKAgICAgIDA//8AURsNACAEQoCAgICAgCCEIQcgAyEBDAILAkAgASANQoCAgICAgMD//wCFhEIAUg0AAkAgAyAChFBFDQBCgICAgICA4P//ACEHQgAhAQwDCyAHQoCAgICAgMD//wCEIQdCACEBDAILAkAgAyACQoCAgICAgMD//wCFhEIAUg0AIAEgDYQhAkIAIQECQCACUEUNAEKAgICAgIDg//8AIQcMAwsgB0KAgICAgIDA//8AhCEHDAILAkAgASANhEIAUg0AQgAhAQwCCwJAIAMgAoRCAFINAEIAIQEMAgtBACEMAkAgDUL///////8/Vg0AIAVB0ABqIAEgCCABIAggCFAiDBt5IAxBBnStfKciDEFxahD0goCAAEEQIAxrIQwgBSkDWCIIQiCIIQkgBSkDUCEBCyACQv///////z9WDQAgBUHAAGogAyAGIAMgBiAGUCIOG3kgDkEGdK18pyIOQXFqEPSCgIAAIAwgDmtBEGohDCAFKQNIIQYgBSkDQCEDCyADQg+GIg1CgID+/w+DIgIgAUIgiCIEfiIPIA1CIIgiDSABQv////8PgyIBfnwiEEIghiIRIAIgAX58IhIgEVStIAIgCEL/////D4MiCH4iEyANIAR+fCIRIANCMYggBkIPhiIUhEL/////D4MiAyABfnwiFSAQQiCIIBAgD1StQiCGhHwiECACIAlCgIAEhCIGfiIWIA0gCH58IgkgFEIgiEKAgICACIQiAiABfnwiDyADIAR+fCIUQiCGfCIXfCEBIAsgCmogDGpBgYB/aiEKAkACQCACIAR+IhggDSAGfnwiBCAYVK0gBCADIAh+fCINIARUrXwgAiAGfnwgDSARIBNUrSAVIBFUrXx8IgQgDVStfCADIAZ+IgMgAiAIfnwiAiADVK1CIIYgAkIgiIR8IAQgAkIghnwiAiAEVK18IAIgFEIgiCAJIBZUrSAPIAlUrXwgFCAPVK18QiCGhHwiBCACVK18IAQgECAVVK0gFyAQVK18fCICIARUrXwiBEKAgICAgIDAAINQDQAgCkEBaiEKDAELIBJCP4ghAyAEQgGGIAJCP4iEIQQgAkIBhiABQj+IhCECIBJCAYYhEiADIAFCAYaEIQELAkAgCkH//wFIDQAgB0KAgICAgIDA//8AhCEHQgAhAQwBCwJAAkAgCkEASg0AAkBBASAKayILQf8ASw0AIAVBMGogEiABIApB/wBqIgoQ9IKAgAAgBUEgaiACIAQgChD0goCAACAFQRBqIBIgASALEP6CgIAAIAUgAiAEIAsQ/oKAgAAgBSkDICAFKQMQhCAFKQMwIAUpAziEQgBSrYQhEiAFKQMoIAUpAxiEIQEgBSkDCCEEIAUpAwAhAgwCC0IAIQEMAgsgCq1CMIYgBEL///////8/g4QhBAsgBCAHhCEHAkAgElAgAUJ/VSABQoCAgICAgICAgH9RGw0AIAcgAkIBfCIBUK18IQcMAQsCQCASIAFCgICAgICAgICAf4WEQgBRDQAgAiEBDAELIAcgAiACQgGDfCIBIAJUrXwhBwsgACABNwMAIAAgBzcDCCAFQeAAaiSAgICAAAt1AQF+IAAgBCABfiACIAN+fCADQiCIIgIgAUIgiCIEfnwgA0L/////D4MiAyABQv////8PgyIBfiIFQiCIIAMgBH58IgNCIIh8IANC/////w+DIAIgAX58IgFCIIh8NwMIIAAgAUIghiAFQv////8Pg4Q3AwALVAEBfyOAgICAAEEQayIFJICAgIAAIAUgASACIAMgBEKAgICAgICAgIB/hRDzgoCAACAFKQMAIQQgACAFKQMINwMIIAAgBDcDACAFQRBqJICAgIAAC5sEAwF/An4EfyOAgICAAEEgayICJICAgIAAIAFC////////P4MhAwJAAkAgAUIwiEL//wGDIgSnIgVB/4d/akH9D0sNACAAQjyIIANCBIaEIQMgBUGAiH9qrSEEAkACQCAAQv//////////D4MiAEKBgICAgICAgAhUDQAgA0IBfCEDDAELIABCgICAgICAgIAIUg0AIANCAYMgA3whAwtCACADIANC/////////wdWIgUbIQAgBa0gBHwhAwwBCwJAIAAgA4RQDQAgBEL//wFSDQAgAEI8iCADQgSGhEKAgICAgICABIQhAEL/DyEDDAELAkAgBUH+hwFNDQBC/w8hA0IAIQAMAQsCQEGA+ABBgfgAIARQIgYbIgcgBWsiCEHwAEwNAEIAIQBCACEDDAELIAJBEGogACADIANCgICAgICAwACEIAYbIgNBgAEgCGsQ9IKAgAAgAiAAIAMgCBD+goCAACACKQMAIgNCPIggAikDCEIEhoQhAAJAAkAgA0L//////////w+DIAcgBUcgAikDECACKQMYhEIAUnGthCIDQoGAgICAgICACFQNACAAQgF8IQAMAQsgA0KAgICAgICAgAhSDQAgAEIBgyAAfCEACyAAQoCAgICAgIAIhSAAIABC/////////wdWIgUbIQAgBa0hAwsgAkEgaiSAgICAACADQjSGIAFCgICAgICAgICAf4OEIACEvwsnAAJAIABFDQBB0oWEgABBuomEgABBGEG6lISAABCAgICAAAALQQELAgALCgAgACSAgICAAAsaAQJ/I4CAgIAAIABrQXBxIgEkgICAgAAgAQsIACOAgICAAAsgAEGAgISAACSCgICAAEGAgICAAEEPakFwcSSBgICAAAsPACOAgICAACOBgICAAGsLCAAjgoCAgAALCAAjgYCAgAALC9FKAgBBgIAEC9BGaW50ZW5zaXR5AGluZmluaXR5AEJpbmQgZ3JvdXAgbGlzdCBhdCBmdWxsIGNhcGFjaXR5AFNjZW5lIG1lc2ggbGlzdCByZWFjaGVkIGZ1bGwgY2FwYWNpdHkAQ291bGRuJ3QgcmVhZCBlbnRpcmUgZmlsZSBpbnRvIG1lbW9yeQBDb3VsZG4ndCBhbGxvY2F0ZSBtZW1vcnkAS0hSX21hdGVyaWFsc19hbmlzb3Ryb3B5AG1hdHJpeABpbmRleABtYXgALSsgICAwWDB4AC0wWCswWCAwWC0weCsweCAweABidWZmZXJWaWV3AHlmb3YAS0hSX3RleHR1cmVfYmFzaXN1ACVzICVsdQBvdXRwdXQAaW5wdXQAc3BvdABjb3VudABwb2ludABLSFJfbWF0ZXJpYWxzX3VubGl0AGNvcHlyaWdodABsaWdodABhc3NldABvZmZzZXQAYnl0ZU9mZnNldAB0YXJnZXQAS0hSX21hdGVyaWFsc19jbGVhcmNvYXQAYnVmZmVyVmlld3MAam9pbnRzAEtIUl9tYXRlcmlhbHNfdmFyaWFudHMAbGlnaHRzAHdlaWdodHMAdGFyZ2V0cwBLSFJfbWF0ZXJpYWxzX3BiclNwZWN1bGFyR2xvc3NpbmVzcwBwYnJNZXRhbGxpY1JvdWdobmVzcwBhY2Nlc3NvcnMAc2FtcGxlcnMAYnVmZmVycwBhbmltYXRpb25zAGV4dGVuc2lvbnMAc2tpbnMAY2hhbm5lbHMAbWF0ZXJpYWxzAG1hcHBpbmdzAHByaW1pdGl2ZXMAdmFsdWVzAGF0dHJpYnV0ZXMAdGV4dHVyZXMAc2NlbmVzAHRhcmdldE5hbWVzAG1lc2hlcwBpbWFnZXMAbm9kZXMAaW52ZXJzZUJpbmRNYXRyaWNlcwBpbmRpY2VzAGNhbnZhcwBleHRyYXMAY2FtZXJhcwAlcwBkZXNjcmlwdG9yID09IG51bGxwdHIAY2xlYXJjb2F0RmFjdG9yAHRoaWNrbmVzc0ZhY3RvcgBnbG9zc2luZXNzRmFjdG9yAHJvdWdobmVzc0ZhY3RvcgBjbGVhcmNvYXRSb3VnaG5lc3NGYWN0b3IAc2hlZW5Sb3VnaG5lc3NGYWN0b3IAc3BlY3VsYXJDb2xvckZhY3RvcgBkaWZmdXNlVHJhbnNtaXNzaW9uQ29sb3JGYWN0b3IAc2hlZW5Db2xvckZhY3RvcgBiYXNlQ29sb3JGYWN0b3IAc3BlY3VsYXJGYWN0b3IAdHJhbnNtaXNzaW9uRmFjdG9yAGRpZmZ1c2VUcmFuc21pc3Npb25GYWN0b3IAZW1pc3NpdmVGYWN0b3IAZGlmZnVzZUZhY3RvcgBpcmlkZXNjZW5jZUZhY3RvcgBtZXRhbGxpY0ZhY3RvcgBnZW5lcmF0b3IAY29sb3IAYXR0ZW51YXRpb25Db2xvcgBLSFJfbWF0ZXJpYWxzX2lvcgBpcmlkZXNjZW5jZUlvcgBmaWx0ZXIAbWluRmlsdGVyAG1hZ0ZpbHRlcgBzYW1wbGVyAGJ1ZmZlcgBTaGFkZXIAS0hSX21hdGVyaWFsc19zcGVjdWxhcgB6ZmFyAHpuZWFyAC9lbXNkay9lbXNjcmlwdGVuL3N5c3RlbS9saWIvd2ViZ3B1L3dlYmdwdS5jcHAARVhUX3RleHR1cmVfd2VicABhc3BlY3RSYXRpbwBza2VsZXRvbgByb3RhdGlvbgBhbmlzb3Ryb3B5Um90YXRpb24AdHJhbnNsYXRpb24AaW50ZXJwb2xhdGlvbgBLSFJfbWF0ZXJpYWxzX3RyYW5zbWlzc2lvbgBLSFJfbWF0ZXJpYWxzX2RpZmZ1c2VfdHJhbnNtaXNzaW9uAEVYVF9tZXNob3B0X2NvbXByZXNzaW9uAEtIUl9kcmFjb19tZXNoX2NvbXByZXNzaW9uAHZlcnNpb24AS0hSX21hdGVyaWFsc19kaXNwZXJzaW9uAG1pblZlcnNpb24AbWluAHNraW4AdnNfbWFpbgBmc19tYWluAGNoaWxkcmVuAEtIUl9tYXRlcmlhbHNfc2hlZW4AbmFuAGlyaWRlc2NlbmNlVGhpY2tuZXNzTWF4aW11bQBpcmlkZXNjZW5jZVRoaWNrbmVzc01pbmltdW0AS0hSX3RleHR1cmVfdHJhbnNmb3JtAC4vcnVudGltZS9hc3NldHMvc2hhZGVyL3NoYWRlci5wYnIud2dzbAAuL3J1bnRpbWUvYXNzZXRzL3NoYWRlci9zaGFkZXIuZ3JpZC53Z3NsAEtIUl9saWdodHNfcHVuY3R1YWwAZGlyZWN0aW9uYWwAbWF0ZXJpYWwAdXJpAEtIUl9tYXRlcmlhbHNfZW1pc3NpdmVfc3RyZW5ndGgAYW5pc290cm9weVN0cmVuZ3RoAGVtaXNzaXZlU3RyZW5ndGgAYnl0ZUxlbmd0aABwYXRoAG1lc2gARVhUX21lc2hfZ3B1X2luc3RhbmNpbmcAeW1hZwB4bWFnAC4vcmVzb3VyY2VzL2Fzc2V0cy9nbHRmL2N1YmUuZ2x0ZgBpbmYAYWxwaGFDdXRvZmYAcGVyc3BlY3RpdmUAU2hhZGVyIGhhcyBubyBkZXZpY2Ugb3IgcXVldWUATWVzaCBoYXMgbm8gZGV2aWNlIG9yIHF1ZXVlAHNwYXJzZQBhbmlzb3Ryb3B5VGV4dHVyZQBjbGVhcmNvYXRUZXh0dXJlAHRoaWNrbmVzc1RleHR1cmUAaXJpZGVzY2VuY2VUaGlja25lc3NUZXh0dXJlAHNwZWN1bGFyR2xvc3NpbmVzc1RleHR1cmUAY2xlYXJjb2F0Um91Z2huZXNzVGV4dHVyZQBzaGVlblJvdWdobmVzc1RleHR1cmUAbWV0YWxsaWNSb3VnaG5lc3NUZXh0dXJlAHNwZWN1bGFyQ29sb3JUZXh0dXJlAGRpZmZ1c2VUcmFuc21pc3Npb25Db2xvclRleHR1cmUAc2hlZW5Db2xvclRleHR1cmUAYmFzZUNvbG9yVGV4dHVyZQBzcGVjdWxhclRleHR1cmUAb2NjbHVzaW9uVGV4dHVyZQB0cmFuc21pc3Npb25UZXh0dXJlAGRpZmZ1c2VUcmFuc21pc3Npb25UZXh0dXJlAG5vcm1hbFRleHR1cmUAY2xlYXJjb2F0Tm9ybWFsVGV4dHVyZQBlbWlzc2l2ZVRleHR1cmUAZGlmZnVzZVRleHR1cmUAaXJpZGVzY2VuY2VUZXh0dXJlAHR5cGUAY29tcG9uZW50VHlwZQBtaW1lVHlwZQBzY2VuZQBLSFJfbWF0ZXJpYWxzX3ZvbHVtZQBuYW1lAG91dGVyQ29uZUFuZ2xlAGlubmVyQ29uZUFuZ2xlAHNjYWxlAHJhbmdlAG5vZGUAbW9kZQBhbHBoYU1vZGUAYnl0ZVN0cmlkZQBzb3VyY2UAS0hSX21hdGVyaWFsc19pcmlkZXNjZW5jZQB3Z3B1Q3JlYXRlSW5zdGFuY2UAYXR0ZW51YXRpb25EaXN0YW5jZQBtYXN0ZXJfY3ViZQB0ZXhDb29yZABncmlkAG5vcm1hbGl6ZWQAZXh0ZW5zaW9uc1VzZWQAZXh0ZW5zaW9uc1JlcXVpcmVkAGRvdWJsZVNpZGVkAG9ydGhvZ3JhcGhpYwByYgByd2EAY2FtZXJhAHdyYXBUAFRBTkdFTlQAd3JhcFMASk9JTlRTAFdFSUdIVFMAQVRUUklCVVRFUwBUUklBTkdMRVMASU5ESUNFUwBDT0xPUgBTQ0FMQVIATElORUFSAFNURVAAUE9TSVRJT04AUVVBVEVSTklPTgBOQU4AT0NUQUhFRFJBTABOT1JNQUwARVhQT05FTlRJQUwATUFTSwBJTkYAT1BBUVVFAE5PTkUAQ1VCSUNTUExJTkUAVEVYQ09PUkQAQkxFTkQAZGF0YToATUFUNABWRUM0ADtiYXNlNjQATUFUMwBWRUMzAE1BVDIAVkVDMgA6Ly8ALgAobnVsbCkATWVzaCBoYXMgbm8gZGV2aWNlIG9yIHF1ZXVlIABMIABHTFRGIGxvYWRpbmcgYWJvcnRlZCwgb3V0IG9mIG1lbW9yeQoARmFpbGVkIHRvIGV4cGFuZCBtZXNoIGxpc3QKAG1lc2ggbmFtZTogJXMKAEdMVEYgbG9hZGluZyBhYm9ydGVkLCB1bmhhbmRlZCBlcnJvcgoAc2V0IHZlcnRleCBsYXlvdXQ6ICVwCgBidWlsZCBjaGlsZHJlbgoAQ291bGRuJ3QgbG9hZCBmaWxlCgBHTFRGIGZpbGUgbm90IGZvdW5kCgBleHBhbmQKAFdBU00gSU5JVAoASW52YWxpZCBHTFRGIEpTT04KADw8IEdMVEYgRE9ORSA+PgoAAAAAAAAAgD8AAAAAAAAAAAAAgD8AAAAAAAAAAAAAAAAAAIA/AAAAAAAAAAAAAAAAAAAAAAAAAAAAAIA/AAAAAAAAAAAAAAAAAAAAAAAAyEIAAMhCAAAAQgAAAAADAAAABAAAAAQAAAAGAAAAg/miAERObgD8KRUA0VcnAN009QBi28AAPJmVAEGQQwBjUf4Au96rALdh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7QDSAHcA/PRVAAFZTQDgcYAAAAAAAAAAAAAAAABA+yH5PwAAAAAtRHQ+AAAAgJhG+DwAAABgUcx4OwAAAICDG/A5AAAAQCAlejgAAACAIoLjNgAAAAAd82k1ECQBAE5vIGVycm9yIGluZm9ybWF0aW9uAElsbGVnYWwgYnl0ZSBzZXF1ZW5jZQBEb21haW4gZXJyb3IAUmVzdWx0IG5vdCByZXByZXNlbnRhYmxlAE5vdCBhIHR0eQBQZXJtaXNzaW9uIGRlbmllZABPcGVyYXRpb24gbm90IHBlcm1pdHRlZABObyBzdWNoIGZpbGUgb3IgZGlyZWN0b3J5AE5vIHN1Y2ggcHJvY2VzcwBGaWxlIGV4aXN0cwBWYWx1ZSB0b28gbGFyZ2UgZm9yIGRhdGEgdHlwZQBObyBzcGFjZSBsZWZ0IG9uIGRldmljZQBPdXQgb2YgbWVtb3J5AFJlc291cmNlIGJ1c3kASW50ZXJydXB0ZWQgc3lzdGVtIGNhbGwAUmVzb3VyY2UgdGVtcG9yYXJpbHkgdW5hdmFpbGFibGUASW52YWxpZCBzZWVrAENyb3NzLWRldmljZSBsaW5rAFJlYWQtb25seSBmaWxlIHN5c3RlbQBEaXJlY3Rvcnkgbm90IGVtcHR5AENvbm5lY3Rpb24gcmVzZXQgYnkgcGVlcgBPcGVyYXRpb24gdGltZWQgb3V0AENvbm5lY3Rpb24gcmVmdXNlZABIb3N0IGlzIGRvd24ASG9zdCBpcyB1bnJlYWNoYWJsZQBBZGRyZXNzIGluIHVzZQBCcm9rZW4gcGlwZQBJL08gZXJyb3IATm8gc3VjaCBkZXZpY2Ugb3IgYWRkcmVzcwBCbG9jayBkZXZpY2UgcmVxdWlyZWQATm8gc3VjaCBkZXZpY2UATm90IGEgZGlyZWN0b3J5AElzIGEgZGlyZWN0b3J5AFRleHQgZmlsZSBidXN5AEV4ZWMgZm9ybWF0IGVycm9yAEludmFsaWQgYXJndW1lbnQAQXJndW1lbnQgbGlzdCB0b28gbG9uZwBTeW1ib2xpYyBsaW5rIGxvb3AARmlsZW5hbWUgdG9vIGxvbmcAVG9vIG1hbnkgb3BlbiBmaWxlcyBpbiBzeXN0ZW0ATm8gZmlsZSBkZXNjcmlwdG9ycyBhdmFpbGFibGUAQmFkIGZpbGUgZGVzY3JpcHRvcgBObyBjaGlsZCBwcm9jZXNzAEJhZCBhZGRyZXNzAEZpbGUgdG9vIGxhcmdlAFRvbyBtYW55IGxpbmtzAE5vIGxvY2tzIGF2YWlsYWJsZQBSZXNvdXJjZSBkZWFkbG9jayB3b3VsZCBvY2N1cgBTdGF0ZSBub3QgcmVjb3ZlcmFibGUAUHJldmlvdXMgb3duZXIgZGllZABPcGVyYXRpb24gY2FuY2VsZWQARnVuY3Rpb24gbm90IGltcGxlbWVudGVkAE5vIG1lc3NhZ2Ugb2YgZGVzaXJlZCB0eXBlAElkZW50aWZpZXIgcmVtb3ZlZABEZXZpY2Ugbm90IGEgc3RyZWFtAE5vIGRhdGEgYXZhaWxhYmxlAERldmljZSB0aW1lb3V0AE91dCBvZiBzdHJlYW1zIHJlc291cmNlcwBMaW5rIGhhcyBiZWVuIHNldmVyZWQAUHJvdG9jb2wgZXJyb3IAQmFkIG1lc3NhZ2UARmlsZSBkZXNjcmlwdG9yIGluIGJhZCBzdGF0ZQBOb3QgYSBzb2NrZXQARGVzdGluYXRpb24gYWRkcmVzcyByZXF1aXJlZABNZXNzYWdlIHRvbyBsYXJnZQBQcm90b2NvbCB3cm9uZyB0eXBlIGZvciBzb2NrZXQAUHJvdG9jb2wgbm90IGF2YWlsYWJsZQBQcm90b2NvbCBub3Qgc3VwcG9ydGVkAFNvY2tldCB0eXBlIG5vdCBzdXBwb3J0ZWQATm90IHN1cHBvcnRlZABQcm90b2NvbCBmYW1pbHkgbm90IHN1cHBvcnRlZABBZGRyZXNzIGZhbWlseSBub3Qgc3VwcG9ydGVkIGJ5IHByb3RvY29sAEFkZHJlc3Mgbm90IGF2YWlsYWJsZQBOZXR3b3JrIGlzIGRvd24ATmV0d29yayB1bnJlYWNoYWJsZQBDb25uZWN0aW9uIHJlc2V0IGJ5IG5ldHdvcmsAQ29ubmVjdGlvbiBhYm9ydGVkAE5vIGJ1ZmZlciBzcGFjZSBhdmFpbGFibGUAU29ja2V0IGlzIGNvbm5lY3RlZABTb2NrZXQgbm90IGNvbm5lY3RlZABDYW5ub3Qgc2VuZCBhZnRlciBzb2NrZXQgc2h1dGRvd24AT3BlcmF0aW9uIGFscmVhZHkgaW4gcHJvZ3Jlc3MAT3BlcmF0aW9u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';

function getBinarySync(file) {
  if (file == wasmBinaryFile && wasmBinary) {
    return new Uint8Array(wasmBinary);
  }
  var binary = tryParseAsDataURI(file);
  if (binary) {
    return binary;
  }
  if (readBinary) {
    return readBinary(file);
  }
  throw 'both async and sync fetching of the wasm failed';
}

async function getWasmBinary(binaryFile) {

  // Otherwise, getBinarySync should be able to get it synchronously
  return getBinarySync(binaryFile);
}

async function instantiateArrayBuffer(binaryFile, imports) {
  try {
    var binary = await getWasmBinary(binaryFile);
    var instance = await WebAssembly.instantiate(binary, imports);
    return instance;
  } catch (reason) {
    err(`failed to asynchronously prepare wasm: ${reason}`);

    // Warn on some common problems.
    if (isFileURI(wasmBinaryFile)) {
      err(`warning: Loading from a file URI (${wasmBinaryFile}) is not supported in most browsers. See https://emscripten.org/docs/getting_started/FAQ.html#how-do-i-run-a-local-webserver-for-testing-why-does-my-program-stall-in-downloading-or-preparing`);
    }
    abort(reason);
  }
}

async function instantiateAsync(binary, binaryFile, imports) {
  return instantiateArrayBuffer(binaryFile, imports);
}

function getWasmImports() {
  // prepare imports
  return {
    'env': wasmImports,
    'wasi_snapshot_preview1': wasmImports,
  }
}

// Create the wasm instance.
// Receives the wasm imports, returns the exports.
async function createWasm() {
  // Load the wasm module and create an instance of using native support in the JS engine.
  // handle a generated wasm instance, receiving its exports and
  // performing other necessary setup
  /** @param {WebAssembly.Module=} module*/
  function receiveInstance(instance, module) {
    wasmExports = instance.exports;

    

    wasmMemory = wasmExports['memory'];
    
    assert(wasmMemory, 'memory not found in wasm exports');
    updateMemoryViews();

    wasmTable = wasmExports['__indirect_function_table'];
    
    assert(wasmTable, 'table not found in wasm exports');

    addOnInit(wasmExports['__wasm_call_ctors']);

    removeRunDependency('wasm-instantiate');
    return wasmExports;
  }
  // wait for the pthread pool (if any)
  addRunDependency('wasm-instantiate');

  // Prefer streaming instantiation if available.
  // Async compilation can be confusing when an error on the page overwrites Module
  // (for example, if the order of elements is wrong, and the one defining Module is
  // later), so we save Module and check it later.
  var trueModule = Module;
  function receiveInstantiationResult(result) {
    // 'result' is a ResultObject object which has both the module and instance.
    // receiveInstance() will swap in the exports (to Module.asm) so they can be called
    assert(Module === trueModule, 'the Module object should not be replaced during async compilation - perhaps the order of HTML elements is wrong?');
    trueModule = null;
    // TODO: Due to Closure regression https://github.com/google/closure-compiler/issues/3193, the above line no longer optimizes out down to the following line.
    // When the regression is fixed, can restore the above PTHREADS-enabled path.
    return receiveInstance(result['instance']);
  }

  var info = getWasmImports();

  // User shell pages can write their own Module.instantiateWasm = function(imports, successCallback) callback
  // to manually instantiate the Wasm module themselves. This allows pages to
  // run the instantiation parallel to any other async startup actions they are
  // performing.
  // Also pthreads and wasm workers initialize the wasm instance through this
  // path.
  if (Module['instantiateWasm']) {
    try {
      return Module['instantiateWasm'](info, receiveInstance);
    } catch(e) {
      err(`Module.instantiateWasm callback failed with error: ${e}`);
        return false;
    }
  }

    var result = await instantiateAsync(wasmBinary, wasmBinaryFile, info);
    var exports = receiveInstantiationResult(result);
    return exports;
}

// === Body ===
// end include: preamble.js


  class ExitStatus {
      name = 'ExitStatus';
      constructor(status) {
        this.message = `Program terminated with exit(${status})`;
        this.status = status;
      }
    }

  var callRuntimeCallbacks = (callbacks) => {
      while (callbacks.length > 0) {
        // Pass the module as the first argument.
        callbacks.shift()(Module);
      }
    };

  
    /**
     * @param {number} ptr
     * @param {string} type
     */
  function getValue(ptr, type = 'i8') {
    if (type.endsWith('*')) type = '*';
    switch (type) {
      case 'i1': return HEAP8[ptr];
      case 'i8': return HEAP8[ptr];
      case 'i16': return HEAP16[((ptr)>>1)];
      case 'i32': return HEAP32[((ptr)>>2)];
      case 'i64': return HEAP64[((ptr)>>3)];
      case 'float': return HEAPF32[((ptr)>>2)];
      case 'double': return HEAPF64[((ptr)>>3)];
      case '*': return HEAPU32[((ptr)>>2)];
      default: abort(`invalid type for getValue: ${type}`);
    }
  }

  var noExitRuntime = Module['noExitRuntime'] || true;

  var ptrToString = (ptr) => {
      assert(typeof ptr === 'number');
      // With CAN_ADDRESS_2GB or MEMORY64, pointers are already unsigned.
      ptr >>>= 0;
      return '0x' + ptr.toString(16).padStart(8, '0');
    };

  
    /**
     * @param {number} ptr
     * @param {number} value
     * @param {string} type
     */
  function setValue(ptr, value, type = 'i8') {
    if (type.endsWith('*')) type = '*';
    switch (type) {
      case 'i1': HEAP8[ptr] = value; break;
      case 'i8': HEAP8[ptr] = value; break;
      case 'i16': HEAP16[((ptr)>>1)] = value; break;
      case 'i32': HEAP32[((ptr)>>2)] = value; break;
      case 'i64': HEAP64[((ptr)>>3)] = BigInt(value); break;
      case 'float': HEAPF32[((ptr)>>2)] = value; break;
      case 'double': HEAPF64[((ptr)>>3)] = value; break;
      case '*': HEAPU32[((ptr)>>2)] = value; break;
      default: abort(`invalid type for setValue: ${type}`);
    }
  }

  var stackRestore = (val) => __emscripten_stack_restore(val);

  var stackSave = () => _emscripten_stack_get_current();

  var warnOnce = (text) => {
      warnOnce.shown ||= {};
      if (!warnOnce.shown[text]) {
        warnOnce.shown[text] = 1;
        if (ENVIRONMENT_IS_NODE) text = 'warning: ' + text;
        err(text);
      }
    };

  var UTF8Decoder = typeof TextDecoder != 'undefined' ? new TextDecoder() : undefined;
  
    /**
     * Given a pointer 'idx' to a null-terminated UTF8-encoded string in the given
     * array that contains uint8 values, returns a copy of that string as a
     * Javascript String object.
     * heapOrArray is either a regular array, or a JavaScript typed array view.
     * @param {number=} idx
     * @param {number=} maxBytesToRead
     * @return {string}
     */
  var UTF8ArrayToString = (heapOrArray, idx = 0, maxBytesToRead = NaN) => {
      var endIdx = idx + maxBytesToRead;
      var endPtr = idx;
      // TextDecoder needs to know the byte length in advance, it doesn't stop on
      // null terminator by itself.  Also, use the length info to avoid running tiny
      // strings through TextDecoder, since .subarray() allocates garbage.
      // (As a tiny code save trick, compare endPtr against endIdx using a negation,
      // so that undefined/NaN means Infinity)
      while (heapOrArray[endPtr] && !(endPtr >= endIdx)) ++endPtr;
  
      if (endPtr - idx > 16 && heapOrArray.buffer && UTF8Decoder) {
        return UTF8Decoder.decode(heapOrArray.subarray(idx, endPtr));
      }
      var str = '';
      // If building with TextDecoder, we have already computed the string length
      // above, so test loop end condition against that
      while (idx < endPtr) {
        // For UTF8 byte structure, see:
        // http://en.wikipedia.org/wiki/UTF-8#Description
        // https://www.ietf.org/rfc/rfc2279.txt
        // https://tools.ietf.org/html/rfc3629
        var u0 = heapOrArray[idx++];
        if (!(u0 & 0x80)) { str += String.fromCharCode(u0); continue; }
        var u1 = heapOrArray[idx++] & 63;
        if ((u0 & 0xE0) == 0xC0) { str += String.fromCharCode(((u0 & 31) << 6) | u1); continue; }
        var u2 = heapOrArray[idx++] & 63;
        if ((u0 & 0xF0) == 0xE0) {
          u0 = ((u0 & 15) << 12) | (u1 << 6) | u2;
        } else {
          if ((u0 & 0xF8) != 0xF0) warnOnce('Invalid UTF-8 leading byte ' + ptrToString(u0) + ' encountered when deserializing a UTF-8 string in wasm memory to a JS string!');
          u0 = ((u0 & 7) << 18) | (u1 << 12) | (u2 << 6) | (heapOrArray[idx++] & 63);
        }
  
        if (u0 < 0x10000) {
          str += String.fromCharCode(u0);
        } else {
          var ch = u0 - 0x10000;
          str += String.fromCharCode(0xD800 | (ch >> 10), 0xDC00 | (ch & 0x3FF));
        }
      }
      return str;
    };
  
    /**
     * Given a pointer 'ptr' to a null-terminated UTF8-encoded string in the
     * emscripten HEAP, returns a copy of that string as a Javascript String object.
     *
     * @param {number} ptr
     * @param {number=} maxBytesToRead - An optional length that specifies the
     *   maximum number of bytes to read. You can omit this parameter to scan the
     *   string until the first 0 byte. If maxBytesToRead is passed, and the string
     *   at [ptr, ptr+maxBytesToReadr[ contains a null byte in the middle, then the
     *   string will cut short at that byte index (i.e. maxBytesToRead will not
     *   produce a string of exact length [ptr, ptr+maxBytesToRead[) N.B. mixing
     *   frequent uses of UTF8ToString() with and without maxBytesToRead may throw
     *   JS JIT optimizations off, so it is worth to consider consistently using one
     * @return {string}
     */
  var UTF8ToString = (ptr, maxBytesToRead) => {
      assert(typeof ptr == 'number', `UTF8ToString expects a number (got ${typeof ptr})`);
      return ptr ? UTF8ArrayToString(HEAPU8, ptr, maxBytesToRead) : '';
    };
  var ___assert_fail = (condition, filename, line, func) =>
      abort(`Assertion failed: ${UTF8ToString(condition)}, at: ` + [filename ? UTF8ToString(filename) : 'unknown filename', line, func ? UTF8ToString(func) : 'unknown function']);

  /** @suppress {duplicate } */
  var syscallGetVarargI = () => {
      assert(SYSCALLS.varargs != undefined);
      // the `+` prepended here is necessary to convince the JSCompiler that varargs is indeed a number.
      var ret = HEAP32[((+SYSCALLS.varargs)>>2)];
      SYSCALLS.varargs += 4;
      return ret;
    };
  var syscallGetVarargP = syscallGetVarargI;
  
  
  var PATH = {
  isAbs:(path) => path.charAt(0) === '/',
  splitPath:(filename) => {
        var splitPathRe = /^(\/?|)([\s\S]*?)((?:\.{1,2}|[^\/]+?|)(\.[^.\/]*|))(?:[\/]*)$/;
        return splitPathRe.exec(filename).slice(1);
      },
  normalizeArray:(parts, allowAboveRoot) => {
        // if the path tries to go above the root, `up` ends up > 0
        var up = 0;
        for (var i = parts.length - 1; i >= 0; i--) {
          var last = parts[i];
          if (last === '.') {
            parts.splice(i, 1);
          } else if (last === '..') {
            parts.splice(i, 1);
            up++;
          } else if (up) {
            parts.splice(i, 1);
            up--;
          }
        }
        // if the path is allowed to go above the root, restore leading ..s
        if (allowAboveRoot) {
          for (; up; up--) {
            parts.unshift('..');
          }
        }
        return parts;
      },
  normalize:(path) => {
        var isAbsolute = PATH.isAbs(path),
            trailingSlash = path.substr(-1) === '/';
        // Normalize the path
        path = PATH.normalizeArray(path.split('/').filter((p) => !!p), !isAbsolute).join('/');
        if (!path && !isAbsolute) {
          path = '.';
        }
        if (path && trailingSlash) {
          path += '/';
        }
        return (isAbsolute ? '/' : '') + path;
      },
  dirname:(path) => {
        var result = PATH.splitPath(path),
            root = result[0],
            dir = result[1];
        if (!root && !dir) {
          // No dirname whatsoever
          return '.';
        }
        if (dir) {
          // It has a dirname, strip trailing slash
          dir = dir.substr(0, dir.length - 1);
        }
        return root + dir;
      },
  basename:(path) => path && path.match(/([^\/]+|\/)\/*$/)[1],
  join:(...paths) => PATH.normalize(paths.join('/')),
  join2:(l, r) => PATH.normalize(l + '/' + r),
  };
  
  var initRandomFill = () => {
      // This block is not needed on v19+ since crypto.getRandomValues is builtin
      if (ENVIRONMENT_IS_NODE) {
        var nodeCrypto = require('crypto');
        return (view) => nodeCrypto.randomFillSync(view);
      }
  
      return (view) => crypto.getRandomValues(view);
    };
  var randomFill = (view) => {
      // Lazily init on the first invocation.
      (randomFill = initRandomFill())(view);
    };
  
  
  
  var PATH_FS = {
  resolve:(...args) => {
        var resolvedPath = '',
          resolvedAbsolute = false;
        for (var i = args.length - 1; i >= -1 && !resolvedAbsolute; i--) {
          var path = (i >= 0) ? args[i] : FS.cwd();
          // Skip empty and invalid entries
          if (typeof path != 'string') {
            throw new TypeError('Arguments to path.resolve must be strings');
          } else if (!path) {
            return ''; // an invalid portion invalidates the whole thing
          }
          resolvedPath = path + '/' + resolvedPath;
          resolvedAbsolute = PATH.isAbs(path);
        }
        // At this point the path should be resolved to a full absolute path, but
        // handle relative paths to be safe (might happen when process.cwd() fails)
        resolvedPath = PATH.normalizeArray(resolvedPath.split('/').filter((p) => !!p), !resolvedAbsolute).join('/');
        return ((resolvedAbsolute ? '/' : '') + resolvedPath) || '.';
      },
  relative:(from, to) => {
        from = PATH_FS.resolve(from).substr(1);
        to = PATH_FS.resolve(to).substr(1);
        function trim(arr) {
          var start = 0;
          for (; start < arr.length; start++) {
            if (arr[start] !== '') break;
          }
          var end = arr.length - 1;
          for (; end >= 0; end--) {
            if (arr[end] !== '') break;
          }
          if (start > end) return [];
          return arr.slice(start, end - start + 1);
        }
        var fromParts = trim(from.split('/'));
        var toParts = trim(to.split('/'));
        var length = Math.min(fromParts.length, toParts.length);
        var samePartsLength = length;
        for (var i = 0; i < length; i++) {
          if (fromParts[i] !== toParts[i]) {
            samePartsLength = i;
            break;
          }
        }
        var outputParts = [];
        for (var i = samePartsLength; i < fromParts.length; i++) {
          outputParts.push('..');
        }
        outputParts = outputParts.concat(toParts.slice(samePartsLength));
        return outputParts.join('/');
      },
  };
  
  
  
  var FS_stdin_getChar_buffer = [];
  
  var lengthBytesUTF8 = (str) => {
      var len = 0;
      for (var i = 0; i < str.length; ++i) {
        // Gotcha: charCodeAt returns a 16-bit word that is a UTF-16 encoded code
        // unit, not a Unicode code point of the character! So decode
        // UTF16->UTF32->UTF8.
        // See http://unicode.org/faq/utf_bom.html#utf16-3
        var c = str.charCodeAt(i); // possibly a lead surrogate
        if (c <= 0x7F) {
          len++;
        } else if (c <= 0x7FF) {
          len += 2;
        } else if (c >= 0xD800 && c <= 0xDFFF) {
          len += 4; ++i;
        } else {
          len += 3;
        }
      }
      return len;
    };
  
  var stringToUTF8Array = (str, heap, outIdx, maxBytesToWrite) => {
      assert(typeof str === 'string', `stringToUTF8Array expects a string (got ${typeof str})`);
      // Parameter maxBytesToWrite is not optional. Negative values, 0, null,
      // undefined and false each don't write out any bytes.
      if (!(maxBytesToWrite > 0))
        return 0;
  
      var startIdx = outIdx;
      var endIdx = outIdx + maxBytesToWrite - 1; // -1 for string null terminator.
      for (var i = 0; i < str.length; ++i) {
        // Gotcha: charCodeAt returns a 16-bit word that is a UTF-16 encoded code
        // unit, not a Unicode code point of the character! So decode
        // UTF16->UTF32->UTF8.
        // See http://unicode.org/faq/utf_bom.html#utf16-3
        // For UTF8 byte structure, see http://en.wikipedia.org/wiki/UTF-8#Description
        // and https://www.ietf.org/rfc/rfc2279.txt
        // and https://tools.ietf.org/html/rfc3629
        var u = str.charCodeAt(i); // possibly a lead surrogate
        if (u >= 0xD800 && u <= 0xDFFF) {
          var u1 = str.charCodeAt(++i);
          u = 0x10000 + ((u & 0x3FF) << 10) | (u1 & 0x3FF);
        }
        if (u <= 0x7F) {
          if (outIdx >= endIdx) break;
          heap[outIdx++] = u;
        } else if (u <= 0x7FF) {
          if (outIdx + 1 >= endIdx) break;
          heap[outIdx++] = 0xC0 | (u >> 6);
          heap[outIdx++] = 0x80 | (u & 63);
        } else if (u <= 0xFFFF) {
          if (outIdx + 2 >= endIdx) break;
          heap[outIdx++] = 0xE0 | (u >> 12);
          heap[outIdx++] = 0x80 | ((u >> 6) & 63);
          heap[outIdx++] = 0x80 | (u & 63);
        } else {
          if (outIdx + 3 >= endIdx) break;
          if (u > 0x10FFFF) warnOnce('Invalid Unicode code point ' + ptrToString(u) + ' encountered when serializing a JS string to a UTF-8 string in wasm memory! (Valid unicode code points should be in range 0-0x10FFFF).');
          heap[outIdx++] = 0xF0 | (u >> 18);
          heap[outIdx++] = 0x80 | ((u >> 12) & 63);
          heap[outIdx++] = 0x80 | ((u >> 6) & 63);
          heap[outIdx++] = 0x80 | (u & 63);
        }
      }
      // Null-terminate the pointer to the buffer.
      heap[outIdx] = 0;
      return outIdx - startIdx;
    };
  /** @type {function(string, boolean=, number=)} */
  function intArrayFromString(stringy, dontAddNull, length) {
    var len = length > 0 ? length : lengthBytesUTF8(stringy)+1;
    var u8array = new Array(len);
    var numBytesWritten = stringToUTF8Array(stringy, u8array, 0, u8array.length);
    if (dontAddNull) u8array.length = numBytesWritten;
    return u8array;
  }
  var FS_stdin_getChar = () => {
      if (!FS_stdin_getChar_buffer.length) {
        var result = null;
        if (ENVIRONMENT_IS_NODE) {
          // we will read data by chunks of BUFSIZE
          var BUFSIZE = 256;
          var buf = Buffer.alloc(BUFSIZE);
          var bytesRead = 0;
  
          // For some reason we must suppress a closure warning here, even though
          // fd definitely exists on process.stdin, and is even the proper way to
          // get the fd of stdin,
          // https://github.com/nodejs/help/issues/2136#issuecomment-523649904
          // This started to happen after moving this logic out of library_tty.js,
          // so it is related to the surrounding code in some unclear manner.
          /** @suppress {missingProperties} */
          var fd = process.stdin.fd;
  
          try {
            bytesRead = fs.readSync(fd, buf, 0, BUFSIZE);
          } catch(e) {
            // Cross-platform differences: on Windows, reading EOF throws an
            // exception, but on other OSes, reading EOF returns 0. Uniformize
            // behavior by treating the EOF exception to return 0.
            if (e.toString().includes('EOF')) bytesRead = 0;
            else throw e;
          }
  
          if (bytesRead > 0) {
            result = buf.slice(0, bytesRead).toString('utf-8');
          }
        } else
        if (typeof window != 'undefined' &&
          typeof window.prompt == 'function') {
          // Browser.
          result = window.prompt('Input: ');  // returns null on cancel
          if (result !== null) {
            result += '\n';
          }
        } else
        {}
        if (!result) {
          return null;
        }
        FS_stdin_getChar_buffer = intArrayFromString(result, true);
      }
      return FS_stdin_getChar_buffer.shift();
    };
  var TTY = {
  ttys:[],
  init() {
        // https://github.com/emscripten-core/emscripten/pull/1555
        // if (ENVIRONMENT_IS_NODE) {
        //   // currently, FS.init does not distinguish if process.stdin is a file or TTY
        //   // device, it always assumes it's a TTY device. because of this, we're forcing
        //   // process.stdin to UTF8 encoding to at least make stdin reading compatible
        //   // with text files until FS.init can be refactored.
        //   process.stdin.setEncoding('utf8');
        // }
      },
  shutdown() {
        // https://github.com/emscripten-core/emscripten/pull/1555
        // if (ENVIRONMENT_IS_NODE) {
        //   // inolen: any idea as to why node -e 'process.stdin.read()' wouldn't exit immediately (with process.stdin being a tty)?
        //   // isaacs: because now it's reading from the stream, you've expressed interest in it, so that read() kicks off a _read() which creates a ReadReq operation
        //   // inolen: I thought read() in that case was a synchronous operation that just grabbed some amount of buffered data if it exists?
        //   // isaacs: it is. but it also triggers a _read() call, which calls readStart() on the handle
        //   // isaacs: do process.stdin.pause() and i'd think it'd probably close the pending call
        //   process.stdin.pause();
        // }
      },
  register(dev, ops) {
        TTY.ttys[dev] = { input: [], output: [], ops: ops };
        FS.registerDevice(dev, TTY.stream_ops);
      },
  stream_ops:{
  open(stream) {
          var tty = TTY.ttys[stream.node.rdev];
          if (!tty) {
            throw new FS.ErrnoError(43);
          }
          stream.tty = tty;
          stream.seekable = false;
        },
  close(stream) {
          // flush any pending line data
          stream.tty.ops.fsync(stream.tty);
        },
  fsync(stream) {
          stream.tty.ops.fsync(stream.tty);
        },
  read(stream, buffer, offset, length, pos /* ignored */) {
          if (!stream.tty || !stream.tty.ops.get_char) {
            throw new FS.ErrnoError(60);
          }
          var bytesRead = 0;
          for (var i = 0; i < length; i++) {
            var result;
            try {
              result = stream.tty.ops.get_char(stream.tty);
            } catch (e) {
              throw new FS.ErrnoError(29);
            }
            if (result === undefined && bytesRead === 0) {
              throw new FS.ErrnoError(6);
            }
            if (result === null || result === undefined) break;
            bytesRead++;
            buffer[offset+i] = result;
          }
          if (bytesRead) {
            stream.node.atime = Date.now();
          }
          return bytesRead;
        },
  write(stream, buffer, offset, length, pos) {
          if (!stream.tty || !stream.tty.ops.put_char) {
            throw new FS.ErrnoError(60);
          }
          try {
            for (var i = 0; i < length; i++) {
              stream.tty.ops.put_char(stream.tty, buffer[offset+i]);
            }
          } catch (e) {
            throw new FS.ErrnoError(29);
          }
          if (length) {
            stream.node.mtime = stream.node.ctime = Date.now();
          }
          return i;
        },
  },
  default_tty_ops:{
  get_char(tty) {
          return FS_stdin_getChar();
        },
  put_char(tty, val) {
          if (val === null || val === 10) {
            out(UTF8ArrayToString(tty.output));
            tty.output = [];
          } else {
            if (val != 0) tty.output.push(val); // val == 0 would cut text output off in the middle.
          }
        },
  fsync(tty) {
          if (tty.output && tty.output.length > 0) {
            out(UTF8ArrayToString(tty.output));
            tty.output = [];
          }
        },
  ioctl_tcgets(tty) {
          // typical setting
          return {
            c_iflag: 25856,
            c_oflag: 5,
            c_cflag: 191,
            c_lflag: 35387,
            c_cc: [
              0x03, 0x1c, 0x7f, 0x15, 0x04, 0x00, 0x01, 0x00, 0x11, 0x13, 0x1a, 0x00,
              0x12, 0x0f, 0x17, 0x16, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
              0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            ]
          };
        },
  ioctl_tcsets(tty, optional_actions, data) {
          // currently just ignore
          return 0;
        },
  ioctl_tiocgwinsz(tty) {
          return [24, 80];
        },
  },
  default_tty1_ops:{
  put_char(tty, val) {
          if (val === null || val === 10) {
            err(UTF8ArrayToString(tty.output));
            tty.output = [];
          } else {
            if (val != 0) tty.output.push(val);
          }
        },
  fsync(tty) {
          if (tty.output && tty.output.length > 0) {
            err(UTF8ArrayToString(tty.output));
            tty.output = [];
          }
        },
  },
  };
  
  
  var zeroMemory = (address, size) => {
      HEAPU8.fill(0, address, address + size);
    };
  
  var alignMemory = (size, alignment) => {
      assert(alignment, "alignment argument is required");
      return Math.ceil(size / alignment) * alignment;
    };
  var mmapAlloc = (size) => {
      abort('internal error: mmapAlloc called but `emscripten_builtin_memalign` native symbol not exported');
    };
  var MEMFS = {
  ops_table:null,
  mount(mount) {
        return MEMFS.createNode(null, '/', 16895, 0);
      },
  createNode(parent, name, mode, dev) {
        if (FS.isBlkdev(mode) || FS.isFIFO(mode)) {
          // no supported
          throw new FS.ErrnoError(63);
        }
        MEMFS.ops_table ||= {
          dir: {
            node: {
              getattr: MEMFS.node_ops.getattr,
              setattr: MEMFS.node_ops.setattr,
              lookup: MEMFS.node_ops.lookup,
              mknod: MEMFS.node_ops.mknod,
              rename: MEMFS.node_ops.rename,
              unlink: MEMFS.node_ops.unlink,
              rmdir: MEMFS.node_ops.rmdir,
              readdir: MEMFS.node_ops.readdir,
              symlink: MEMFS.node_ops.symlink
            },
            stream: {
              llseek: MEMFS.stream_ops.llseek
            }
          },
          file: {
            node: {
              getattr: MEMFS.node_ops.getattr,
              setattr: MEMFS.node_ops.setattr
            },
            stream: {
              llseek: MEMFS.stream_ops.llseek,
              read: MEMFS.stream_ops.read,
              write: MEMFS.stream_ops.write,
              allocate: MEMFS.stream_ops.allocate,
              mmap: MEMFS.stream_ops.mmap,
              msync: MEMFS.stream_ops.msync
            }
          },
          link: {
            node: {
              getattr: MEMFS.node_ops.getattr,
              setattr: MEMFS.node_ops.setattr,
              readlink: MEMFS.node_ops.readlink
            },
            stream: {}
          },
          chrdev: {
            node: {
              getattr: MEMFS.node_ops.getattr,
              setattr: MEMFS.node_ops.setattr
            },
            stream: FS.chrdev_stream_ops
          }
        };
        var node = FS.createNode(parent, name, mode, dev);
        if (FS.isDir(node.mode)) {
          node.node_ops = MEMFS.ops_table.dir.node;
          node.stream_ops = MEMFS.ops_table.dir.stream;
          node.contents = {};
        } else if (FS.isFile(node.mode)) {
          node.node_ops = MEMFS.ops_table.file.node;
          node.stream_ops = MEMFS.ops_table.file.stream;
          node.usedBytes = 0; // The actual number of bytes used in the typed array, as opposed to contents.length which gives the whole capacity.
          // When the byte data of the file is populated, this will point to either a typed array, or a normal JS array. Typed arrays are preferred
          // for performance, and used by default. However, typed arrays are not resizable like normal JS arrays are, so there is a small disk size
          // penalty involved for appending file writes that continuously grow a file similar to std::vector capacity vs used -scheme.
          node.contents = null; 
        } else if (FS.isLink(node.mode)) {
          node.node_ops = MEMFS.ops_table.link.node;
          node.stream_ops = MEMFS.ops_table.link.stream;
        } else if (FS.isChrdev(node.mode)) {
          node.node_ops = MEMFS.ops_table.chrdev.node;
          node.stream_ops = MEMFS.ops_table.chrdev.stream;
        }
        node.atime = node.mtime = node.ctime = Date.now();
        // add the new node to the parent
        if (parent) {
          parent.contents[name] = node;
          parent.atime = parent.mtime = parent.ctime = node.atime;
        }
        return node;
      },
  getFileDataAsTypedArray(node) {
        if (!node.contents) return new Uint8Array(0);
        if (node.contents.subarray) return node.contents.subarray(0, node.usedBytes); // Make sure to not return excess unused bytes.
        return new Uint8Array(node.contents);
      },
  expandFileStorage(node, newCapacity) {
        var prevCapacity = node.contents ? node.contents.length : 0;
        if (prevCapacity >= newCapacity) return; // No need to expand, the storage was already large enough.
        // Don't expand strictly to the given requested limit if it's only a very small increase, but instead geometrically grow capacity.
        // For small filesizes (<1MB), perform size*2 geometric increase, but for large sizes, do a much more conservative size*1.125 increase to
        // avoid overshooting the allocation cap by a very large margin.
        var CAPACITY_DOUBLING_MAX = 1024 * 1024;
        newCapacity = Math.max(newCapacity, (prevCapacity * (prevCapacity < CAPACITY_DOUBLING_MAX ? 2.0 : 1.125)) >>> 0);
        if (prevCapacity != 0) newCapacity = Math.max(newCapacity, 256); // At minimum allocate 256b for each file when expanding.
        var oldContents = node.contents;
        node.contents = new Uint8Array(newCapacity); // Allocate new storage.
        if (node.usedBytes > 0) node.contents.set(oldContents.subarray(0, node.usedBytes), 0); // Copy old data over to the new storage.
      },
  resizeFileStorage(node, newSize) {
        if (node.usedBytes == newSize) return;
        if (newSize == 0) {
          node.contents = null; // Fully decommit when requesting a resize to zero.
          node.usedBytes = 0;
        } else {
          var oldContents = node.contents;
          node.contents = new Uint8Array(newSize); // Allocate new storage.
          if (oldContents) {
            node.contents.set(oldContents.subarray(0, Math.min(newSize, node.usedBytes))); // Copy old data over to the new storage.
          }
          node.usedBytes = newSize;
        }
      },
  node_ops:{
  getattr(node) {
          var attr = {};
          // device numbers reuse inode numbers.
          attr.dev = FS.isChrdev(node.mode) ? node.id : 1;
          attr.ino = node.id;
          attr.mode = node.mode;
          attr.nlink = 1;
          attr.uid = 0;
          attr.gid = 0;
          attr.rdev = node.rdev;
          if (FS.isDir(node.mode)) {
            attr.size = 4096;
          } else if (FS.isFile(node.mode)) {
            attr.size = node.usedBytes;
          } else if (FS.isLink(node.mode)) {
            attr.size = node.link.length;
          } else {
            attr.size = 0;
          }
          attr.atime = new Date(node.atime);
          attr.mtime = new Date(node.mtime);
          attr.ctime = new Date(node.ctime);
          // NOTE: In our implementation, st_blocks = Math.ceil(st_size/st_blksize),
          //       but this is not required by the standard.
          attr.blksize = 4096;
          attr.blocks = Math.ceil(attr.size / attr.blksize);
          return attr;
        },
  setattr(node, attr) {
          for (const key of ["mode", "atime", "mtime", "ctime"]) {
            if (attr[key] != null) {
              node[key] = attr[key];
            }
          }
          if (attr.size !== undefined) {
            MEMFS.resizeFileStorage(node, attr.size);
          }
        },
  lookup(parent, name) {
          throw new FS.ErrnoError(44);
        },
  mknod(parent, name, mode, dev) {
          return MEMFS.createNode(parent, name, mode, dev);
        },
  rename(old_node, new_dir, new_name) {
          var new_node;
          try {
            new_node = FS.lookupNode(new_dir, new_name);
          } catch (e) {}
          if (new_node) {
            if (FS.isDir(old_node.mode)) {
              // if we're overwriting a directory at new_name, make sure it's empty.
              for (var i in new_node.contents) {
                throw new FS.ErrnoError(55);
              }
            }
            FS.hashRemoveNode(new_node);
          }
          // do the internal rewiring
          delete old_node.parent.contents[old_node.name];
          new_dir.contents[new_name] = old_node;
          old_node.name = new_name;
          new_dir.ctime = new_dir.mtime = old_node.parent.ctime = old_node.parent.mtime = Date.now();
        },
  unlink(parent, name) {
          delete parent.contents[name];
          parent.ctime = parent.mtime = Date.now();
        },
  rmdir(parent, name) {
          var node = FS.lookupNode(parent, name);
          for (var i in node.contents) {
            throw new FS.ErrnoError(55);
          }
          delete parent.contents[name];
          parent.ctime = parent.mtime = Date.now();
        },
  readdir(node) {
          return ['.', '..', ...Object.keys(node.contents)];
        },
  symlink(parent, newname, oldpath) {
          var node = MEMFS.createNode(parent, newname, 0o777 | 40960, 0);
          node.link = oldpath;
          return node;
        },
  readlink(node) {
          if (!FS.isLink(node.mode)) {
            throw new FS.ErrnoError(28);
          }
          return node.link;
        },
  },
  stream_ops:{
  read(stream, buffer, offset, length, position) {
          var contents = stream.node.contents;
          if (position >= stream.node.usedBytes) return 0;
          var size = Math.min(stream.node.usedBytes - position, length);
          assert(size >= 0);
          if (size > 8 && contents.subarray) { // non-trivial, and typed array
            buffer.set(contents.subarray(position, position + size), offset);
          } else {
            for (var i = 0; i < size; i++) buffer[offset + i] = contents[position + i];
          }
          return size;
        },
  write(stream, buffer, offset, length, position, canOwn) {
          // The data buffer should be a typed array view
          assert(!(buffer instanceof ArrayBuffer));
  
          if (!length) return 0;
          var node = stream.node;
          node.mtime = node.ctime = Date.now();
  
          if (buffer.subarray && (!node.contents || node.contents.subarray)) { // This write is from a typed array to a typed array?
            if (canOwn) {
              assert(position === 0, 'canOwn must imply no weird position inside the file');
              node.contents = buffer.subarray(offset, offset + length);
              node.usedBytes = length;
              return length;
            } else if (node.usedBytes === 0 && position === 0) { // If this is a simple first write to an empty file, do a fast set since we don't need to care about old data.
              node.contents = buffer.slice(offset, offset + length);
              node.usedBytes = length;
              return length;
            } else if (position + length <= node.usedBytes) { // Writing to an already allocated and used subrange of the file?
              node.contents.set(buffer.subarray(offset, offset + length), position);
              return length;
            }
          }
  
          // Appending to an existing file and we need to reallocate, or source data did not come as a typed array.
          MEMFS.expandFileStorage(node, position+length);
          if (node.contents.subarray && buffer.subarray) {
            // Use typed array write which is available.
            node.contents.set(buffer.subarray(offset, offset + length), position);
          } else {
            for (var i = 0; i < length; i++) {
             node.contents[position + i] = buffer[offset + i]; // Or fall back to manual write if not.
            }
          }
          node.usedBytes = Math.max(node.usedBytes, position + length);
          return length;
        },
  llseek(stream, offset, whence) {
          var position = offset;
          if (whence === 1) {
            position += stream.position;
          } else if (whence === 2) {
            if (FS.isFile(stream.node.mode)) {
              position += stream.node.usedBytes;
            }
          }
          if (position < 0) {
            throw new FS.ErrnoError(28);
          }
          return position;
        },
  allocate(stream, offset, length) {
          MEMFS.expandFileStorage(stream.node, offset + length);
          stream.node.usedBytes = Math.max(stream.node.usedBytes, offset + length);
        },
  mmap(stream, length, position, prot, flags) {
          if (!FS.isFile(stream.node.mode)) {
            throw new FS.ErrnoError(43);
          }
          var ptr;
          var allocated;
          var contents = stream.node.contents;
          // Only make a new copy when MAP_PRIVATE is specified.
          if (!(flags & 2) && contents && contents.buffer === HEAP8.buffer) {
            // We can't emulate MAP_SHARED when the file is not backed by the
            // buffer we're mapping to (e.g. the HEAP buffer).
            allocated = false;
            ptr = contents.byteOffset;
          } else {
            allocated = true;
            ptr = mmapAlloc(length);
            if (!ptr) {
              throw new FS.ErrnoError(48);
            }
            if (contents) {
              // Try to avoid unnecessary slices.
              if (position > 0 || position + length < contents.length) {
                if (contents.subarray) {
                  contents = contents.subarray(position, position + length);
                } else {
                  contents = Array.prototype.slice.call(contents, position, position + length);
                }
              }
              HEAP8.set(contents, ptr);
            }
          }
          return { ptr, allocated };
        },
  msync(stream, buffer, offset, length, mmapFlags) {
          MEMFS.stream_ops.write(stream, buffer, 0, length, offset, false);
          // should we check if bytesWritten and length are the same?
          return 0;
        },
  },
  };
  
  var asyncLoad = async (url) => {
      var arrayBuffer = await readAsync(url);
      assert(arrayBuffer, `Loading data file "${url}" failed (no arrayBuffer).`);
      return new Uint8Array(arrayBuffer);
    };
  
  
  var FS_createDataFile = (parent, name, fileData, canRead, canWrite, canOwn) => {
      FS.createDataFile(parent, name, fileData, canRead, canWrite, canOwn);
    };
  
  var preloadPlugins = Module['preloadPlugins'] || [];
  var FS_handledByPreloadPlugin = (byteArray, fullname, finish, onerror) => {
      // Ensure plugins are ready.
      if (typeof Browser != 'undefined') Browser.init();
  
      var handled = false;
      preloadPlugins.forEach((plugin) => {
        if (handled) return;
        if (plugin['canHandle'](fullname)) {
          plugin['handle'](byteArray, fullname, finish, onerror);
          handled = true;
        }
      });
      return handled;
    };
  var FS_createPreloadedFile = (parent, name, url, canRead, canWrite, onload, onerror, dontCreateFile, canOwn, preFinish) => {
      // TODO we should allow people to just pass in a complete filename instead
      // of parent and name being that we just join them anyways
      var fullname = name ? PATH_FS.resolve(PATH.join2(parent, name)) : parent;
      var dep = getUniqueRunDependency(`cp ${fullname}`); // might have several active requests for the same fullname
      function processData(byteArray) {
        function finish(byteArray) {
          preFinish?.();
          if (!dontCreateFile) {
            FS_createDataFile(parent, name, byteArray, canRead, canWrite, canOwn);
          }
          onload?.();
          removeRunDependency(dep);
        }
        if (FS_handledByPreloadPlugin(byteArray, fullname, finish, () => {
          onerror?.();
          removeRunDependency(dep);
        })) {
          return;
        }
        finish(byteArray);
      }
      addRunDependency(dep);
      if (typeof url == 'string') {
        asyncLoad(url).then(processData, onerror);
      } else {
        processData(url);
      }
    };
  
  var FS_modeStringToFlags = (str) => {
      var flagModes = {
        'r': 0,
        'r+': 2,
        'w': 512 | 64 | 1,
        'w+': 512 | 64 | 2,
        'a': 1024 | 64 | 1,
        'a+': 1024 | 64 | 2,
      };
      var flags = flagModes[str];
      if (typeof flags == 'undefined') {
        throw new Error(`Unknown file open mode: ${str}`);
      }
      return flags;
    };
  
  var FS_getMode = (canRead, canWrite) => {
      var mode = 0;
      if (canRead) mode |= 292 | 73;
      if (canWrite) mode |= 146;
      return mode;
    };
  
  
  
  
  
  
  var strError = (errno) => UTF8ToString(_strerror(errno));
  
  var ERRNO_CODES = {
      'EPERM': 63,
      'ENOENT': 44,
      'ESRCH': 71,
      'EINTR': 27,
      'EIO': 29,
      'ENXIO': 60,
      'E2BIG': 1,
      'ENOEXEC': 45,
      'EBADF': 8,
      'ECHILD': 12,
      'EAGAIN': 6,
      'EWOULDBLOCK': 6,
      'ENOMEM': 48,
      'EACCES': 2,
      'EFAULT': 21,
      'ENOTBLK': 105,
      'EBUSY': 10,
      'EEXIST': 20,
      'EXDEV': 75,
      'ENODEV': 43,
      'ENOTDIR': 54,
      'EISDIR': 31,
      'EINVAL': 28,
      'ENFILE': 41,
      'EMFILE': 33,
      'ENOTTY': 59,
      'ETXTBSY': 74,
      'EFBIG': 22,
      'ENOSPC': 51,
      'ESPIPE': 70,
      'EROFS': 69,
      'EMLINK': 34,
      'EPIPE': 64,
      'EDOM': 18,
      'ERANGE': 68,
      'ENOMSG': 49,
      'EIDRM': 24,
      'ECHRNG': 106,
      'EL2NSYNC': 156,
      'EL3HLT': 107,
      'EL3RST': 108,
      'ELNRNG': 109,
      'EUNATCH': 110,
      'ENOCSI': 111,
      'EL2HLT': 112,
      'EDEADLK': 16,
      'ENOLCK': 46,
      'EBADE': 113,
      'EBADR': 114,
      'EXFULL': 115,
      'ENOANO': 104,
      'EBADRQC': 103,
      'EBADSLT': 102,
      'EDEADLOCK': 16,
      'EBFONT': 101,
      'ENOSTR': 100,
      'ENODATA': 116,
      'ETIME': 117,
      'ENOSR': 118,
      'ENONET': 119,
      'ENOPKG': 120,
      'EREMOTE': 121,
      'ENOLINK': 47,
      'EADV': 122,
      'ESRMNT': 123,
      'ECOMM': 124,
      'EPROTO': 65,
      'EMULTIHOP': 36,
      'EDOTDOT': 125,
      'EBADMSG': 9,
      'ENOTUNIQ': 126,
      'EBADFD': 127,
      'EREMCHG': 128,
      'ELIBACC': 129,
      'ELIBBAD': 130,
      'ELIBSCN': 131,
      'ELIBMAX': 132,
      'ELIBEXEC': 133,
      'ENOSYS': 52,
      'ENOTEMPTY': 55,
      'ENAMETOOLONG': 37,
      'ELOOP': 32,
      'EOPNOTSUPP': 138,
      'EPFNOSUPPORT': 139,
      'ECONNRESET': 15,
      'ENOBUFS': 42,
      'EAFNOSUPPORT': 5,
      'EPROTOTYPE': 67,
      'ENOTSOCK': 57,
      'ENOPROTOOPT': 50,
      'ESHUTDOWN': 140,
      'ECONNREFUSED': 14,
      'EADDRINUSE': 3,
      'ECONNABORTED': 13,
      'ENETUNREACH': 40,
      'ENETDOWN': 38,
      'ETIMEDOUT': 73,
      'EHOSTDOWN': 142,
      'EHOSTUNREACH': 23,
      'EINPROGRESS': 26,
      'EALREADY': 7,
      'EDESTADDRREQ': 17,
      'EMSGSIZE': 35,
      'EPROTONOSUPPORT': 66,
      'ESOCKTNOSUPPORT': 137,
      'EADDRNOTAVAIL': 4,
      'ENETRESET': 39,
      'EISCONN': 30,
      'ENOTCONN': 53,
      'ETOOMANYREFS': 141,
      'EUSERS': 136,
      'EDQUOT': 19,
      'ESTALE': 72,
      'ENOTSUP': 138,
      'ENOMEDIUM': 148,
      'EILSEQ': 25,
      'EOVERFLOW': 61,
      'ECANCELED': 11,
      'ENOTRECOVERABLE': 56,
      'EOWNERDEAD': 62,
      'ESTRPIPE': 135,
    };
  var FS = {
  root:null,
  mounts:[],
  devices:{
  },
  streams:[],
  nextInode:1,
  nameTable:null,
  currentPath:"/",
  initialized:false,
  ignorePermissions:true,
  ErrnoError:class extends Error {
        name = 'ErrnoError';
        // We set the `name` property to be able to identify `FS.ErrnoError`
        // - the `name` is a standard ECMA-262 property of error objects. Kind of good to have it anyway.
        // - when using PROXYFS, an error can come from an underlying FS
        // as different FS objects have their own FS.ErrnoError each,
        // the test `err instanceof FS.ErrnoError` won't detect an error coming from another filesystem, causing bugs.
        // we'll use the reliable test `err.name == "ErrnoError"` instead
        constructor(errno) {
          super(runtimeInitialized ? strError(errno) : '');
          this.errno = errno;
          for (var key in ERRNO_CODES) {
            if (ERRNO_CODES[key] === errno) {
              this.code = key;
              break;
            }
          }
        }
      },
  filesystems:null,
  syncFSRequests:0,
  readFiles:{
  },
  FSStream:class {
        shared = {};
        get object() {
          return this.node;
        }
        set object(val) {
          this.node = val;
        }
        get isRead() {
          return (this.flags & 2097155) !== 1;
        }
        get isWrite() {
          return (this.flags & 2097155) !== 0;
        }
        get isAppend() {
          return (this.flags & 1024);
        }
        get flags() {
          return this.shared.flags;
        }
        set flags(val) {
          this.shared.flags = val;
        }
        get position() {
          return this.shared.position;
        }
        set position(val) {
          this.shared.position = val;
        }
      },
  FSNode:class {
        node_ops = {};
        stream_ops = {};
        readMode = 292 | 73;
        writeMode = 146;
        mounted = null;
        constructor(parent, name, mode, rdev) {
          if (!parent) {
            parent = this;  // root node sets parent to itself
          }
          this.parent = parent;
          this.mount = parent.mount;
          this.id = FS.nextInode++;
          this.name = name;
          this.mode = mode;
          this.rdev = rdev;
          this.atime = this.mtime = this.ctime = Date.now();
        }
        get read() {
          return (this.mode & this.readMode) === this.readMode;
        }
        set read(val) {
          val ? this.mode |= this.readMode : this.mode &= ~this.readMode;
        }
        get write() {
          return (this.mode & this.writeMode) === this.writeMode;
        }
        set write(val) {
          val ? this.mode |= this.writeMode : this.mode &= ~this.writeMode;
        }
        get isFolder() {
          return FS.isDir(this.mode);
        }
        get isDevice() {
          return FS.isChrdev(this.mode);
        }
      },
  lookupPath(path, opts = {}) {
        if (!path) {
          throw new FS.ErrnoError(44);
        }
        opts.follow_mount ??= true
  
        if (!PATH.isAbs(path)) {
          path = FS.cwd() + '/' + path;
        }
  
        // limit max consecutive symlinks to 40 (SYMLOOP_MAX).
        linkloop: for (var nlinks = 0; nlinks < 40; nlinks++) {
          // split the absolute path
          var parts = path.split('/').filter((p) => !!p);
  
          // start at the root
          var current = FS.root;
          var current_path = '/';
  
          for (var i = 0; i < parts.length; i++) {
            var islast = (i === parts.length-1);
            if (islast && opts.parent) {
              // stop resolving
              break;
            }
  
            if (parts[i] === '.') {
              continue;
            }
  
            if (parts[i] === '..') {
              current_path = PATH.dirname(current_path);
              current = current.parent;
              continue;
            }
  
            current_path = PATH.join2(current_path, parts[i]);
            try {
              current = FS.lookupNode(current, parts[i]);
            } catch (e) {
              // if noent_okay is true, suppress a ENOENT in the last component
              // and return an object with an undefined node. This is needed for
              // resolving symlinks in the path when creating a file.
              if ((e?.errno === 44) && islast && opts.noent_okay) {
                return { path: current_path };
              }
              throw e;
            }
  
            // jump to the mount's root node if this is a mountpoint
            if (FS.isMountpoint(current) && (!islast || opts.follow_mount)) {
              current = current.mounted.root;
            }
  
            // by default, lookupPath will not follow a symlink if it is the final path component.
            // setting opts.follow = true will override this behavior.
            if (FS.isLink(current.mode) && (!islast || opts.follow)) {
              if (!current.node_ops.readlink) {
                throw new FS.ErrnoError(52);
              }
              var link = current.node_ops.readlink(current);
              if (!PATH.isAbs(link)) {
                link = PATH.dirname(current_path) + '/' + link;
              }
              path = link + '/' + parts.slice(i + 1).join('/');
              continue linkloop;
            }
          }
          return { path: current_path, node: current };
        }
        throw new FS.ErrnoError(32);
      },
  getPath(node) {
        var path;
        while (true) {
          if (FS.isRoot(node)) {
            var mount = node.mount.mountpoint;
            if (!path) return mount;
            return mount[mount.length-1] !== '/' ? `${mount}/${path}` : mount + path;
          }
          path = path ? `${node.name}/${path}` : node.name;
          node = node.parent;
        }
      },
  hashName(parentid, name) {
        var hash = 0;
  
        for (var i = 0; i < name.length; i++) {
          hash = ((hash << 5) - hash + name.charCodeAt(i)) | 0;
        }
        return ((parentid + hash) >>> 0) % FS.nameTable.length;
      },
  hashAddNode(node) {
        var hash = FS.hashName(node.parent.id, node.name);
        node.name_next = FS.nameTable[hash];
        FS.nameTable[hash] = node;
      },
  hashRemoveNode(node) {
        var hash = FS.hashName(node.parent.id, node.name);
        if (FS.nameTable[hash] === node) {
          FS.nameTable[hash] = node.name_next;
        } else {
          var current = FS.nameTable[hash];
          while (current) {
            if (current.name_next === node) {
              current.name_next = node.name_next;
              break;
            }
            current = current.name_next;
          }
        }
      },
  lookupNode(parent, name) {
        var errCode = FS.mayLookup(parent);
        if (errCode) {
          throw new FS.ErrnoError(errCode);
        }
        var hash = FS.hashName(parent.id, name);
        for (var node = FS.nameTable[hash]; node; node = node.name_next) {
          var nodeName = node.name;
          if (node.parent.id === parent.id && nodeName === name) {
            return node;
          }
        }
        // if we failed to find it in the cache, call into the VFS
        return FS.lookup(parent, name);
      },
  createNode(parent, name, mode, rdev) {
        assert(typeof parent == 'object')
        var node = new FS.FSNode(parent, name, mode, rdev);
  
        FS.hashAddNode(node);
  
        return node;
      },
  destroyNode(node) {
        FS.hashRemoveNode(node);
      },
  isRoot(node) {
        return node === node.parent;
      },
  isMountpoint(node) {
        return !!node.mounted;
      },
  isFile(mode) {
        return (mode & 61440) === 32768;
      },
  isDir(mode) {
        return (mode & 61440) === 16384;
      },
  isLink(mode) {
        return (mode & 61440) === 40960;
      },
  isChrdev(mode) {
        return (mode & 61440) === 8192;
      },
  isBlkdev(mode) {
        return (mode & 61440) === 24576;
      },
  isFIFO(mode) {
        return (mode & 61440) === 4096;
      },
  isSocket(mode) {
        return (mode & 49152) === 49152;
      },
  flagsToPermissionString(flag) {
        var perms = ['r', 'w', 'rw'][flag & 3];
        if ((flag & 512)) {
          perms += 'w';
        }
        return perms;
      },
  nodePermissions(node, perms) {
        if (FS.ignorePermissions) {
          return 0;
        }
        // return 0 if any user, group or owner bits are set.
        if (perms.includes('r') && !(node.mode & 292)) {
          return 2;
        } else if (perms.includes('w') && !(node.mode & 146)) {
          return 2;
        } else if (perms.includes('x') && !(node.mode & 73)) {
          return 2;
        }
        return 0;
      },
  mayLookup(dir) {
        if (!FS.isDir(dir.mode)) return 54;
        var errCode = FS.nodePermissions(dir, 'x');
        if (errCode) return errCode;
        if (!dir.node_ops.lookup) return 2;
        return 0;
      },
  mayCreate(dir, name) {
        if (!FS.isDir(dir.mode)) {
          return 54;
        }
        try {
          var node = FS.lookupNode(dir, name);
          return 20;
        } catch (e) {
        }
        return FS.nodePermissions(dir, 'wx');
      },
  mayDelete(dir, name, isdir) {
        var node;
        try {
          node = FS.lookupNode(dir, name);
        } catch (e) {
          return e.errno;
        }
        var errCode = FS.nodePermissions(dir, 'wx');
        if (errCode) {
          return errCode;
        }
        if (isdir) {
          if (!FS.isDir(node.mode)) {
            return 54;
          }
          if (FS.isRoot(node) || FS.getPath(node) === FS.cwd()) {
            return 10;
          }
        } else {
          if (FS.isDir(node.mode)) {
            return 31;
          }
        }
        return 0;
      },
  mayOpen(node, flags) {
        if (!node) {
          return 44;
        }
        if (FS.isLink(node.mode)) {
          return 32;
        } else if (FS.isDir(node.mode)) {
          if (FS.flagsToPermissionString(flags) !== 'r' // opening for write
              || (flags & (512 | 64))) { // TODO: check for O_SEARCH? (== search for dir only)
            return 31;
          }
        }
        return FS.nodePermissions(node, FS.flagsToPermissionString(flags));
      },
  checkOpExists(op, err) {
        if (!op) {
          throw new FS.ErrnoError(err);
        }
        return op;
      },
  MAX_OPEN_FDS:4096,
  nextfd() {
        for (var fd = 0; fd <= FS.MAX_OPEN_FDS; fd++) {
          if (!FS.streams[fd]) {
            return fd;
          }
        }
        throw new FS.ErrnoError(33);
      },
  getStreamChecked(fd) {
        var stream = FS.getStream(fd);
        if (!stream) {
          throw new FS.ErrnoError(8);
        }
        return stream;
      },
  getStream:(fd) => FS.streams[fd],
  createStream(stream, fd = -1) {
        assert(fd >= -1);
  
        // clone it, so we can return an instance of FSStream
        stream = Object.assign(new FS.FSStream(), stream);
        if (fd == -1) {
          fd = FS.nextfd();
        }
        stream.fd = fd;
        FS.streams[fd] = stream;
        return stream;
      },
  closeStream(fd) {
        FS.streams[fd] = null;
      },
  dupStream(origStream, fd = -1) {
        var stream = FS.createStream(origStream, fd);
        stream.stream_ops?.dup?.(stream);
        return stream;
      },
  chrdev_stream_ops:{
  open(stream) {
          var device = FS.getDevice(stream.node.rdev);
          // override node's stream ops with the device's
          stream.stream_ops = device.stream_ops;
          // forward the open call
          stream.stream_ops.open?.(stream);
        },
  llseek() {
          throw new FS.ErrnoError(70);
        },
  },
  major:(dev) => ((dev) >> 8),
  minor:(dev) => ((dev) & 0xff),
  makedev:(ma, mi) => ((ma) << 8 | (mi)),
  registerDevice(dev, ops) {
        FS.devices[dev] = { stream_ops: ops };
      },
  getDevice:(dev) => FS.devices[dev],
  getMounts(mount) {
        var mounts = [];
        var check = [mount];
  
        while (check.length) {
          var m = check.pop();
  
          mounts.push(m);
  
          check.push(...m.mounts);
        }
  
        return mounts;
      },
  syncfs(populate, callback) {
        if (typeof populate == 'function') {
          callback = populate;
          populate = false;
        }
  
        FS.syncFSRequests++;
  
        if (FS.syncFSRequests > 1) {
          err(`warning: ${FS.syncFSRequests} FS.syncfs operations in flight at once, probably just doing extra work`);
        }
  
        var mounts = FS.getMounts(FS.root.mount);
        var completed = 0;
  
        function doCallback(errCode) {
          assert(FS.syncFSRequests > 0);
          FS.syncFSRequests--;
          return callback(errCode);
        }
  
        function done(errCode) {
          if (errCode) {
            if (!done.errored) {
              done.errored = true;
              return doCallback(errCode);
            }
            return;
          }
          if (++completed >= mounts.length) {
            doCallback(null);
          }
        };
  
        // sync all mounts
        mounts.forEach((mount) => {
          if (!mount.type.syncfs) {
            return done(null);
          }
          mount.type.syncfs(mount, populate, done);
        });
      },
  mount(type, opts, mountpoint) {
        if (typeof type == 'string') {
          // The filesystem was not included, and instead we have an error
          // message stored in the variable.
          throw type;
        }
        var root = mountpoint === '/';
        var pseudo = !mountpoint;
        var node;
  
        if (root && FS.root) {
          throw new FS.ErrnoError(10);
        } else if (!root && !pseudo) {
          var lookup = FS.lookupPath(mountpoint, { follow_mount: false });
  
          mountpoint = lookup.path;  // use the absolute path
          node = lookup.node;
  
          if (FS.isMountpoint(node)) {
            throw new FS.ErrnoError(10);
          }
  
          if (!FS.isDir(node.mode)) {
            throw new FS.ErrnoError(54);
          }
        }
  
        var mount = {
          type,
          opts,
          mountpoint,
          mounts: []
        };
  
        // create a root node for the fs
        var mountRoot = type.mount(mount);
        mountRoot.mount = mount;
        mount.root = mountRoot;
  
        if (root) {
          FS.root = mountRoot;
        } else if (node) {
          // set as a mountpoint
          node.mounted = mount;
  
          // add the new mount to the current mount's children
          if (node.mount) {
            node.mount.mounts.push(mount);
          }
        }
  
        return mountRoot;
      },
  unmount(mountpoint) {
        var lookup = FS.lookupPath(mountpoint, { follow_mount: false });
  
        if (!FS.isMountpoint(lookup.node)) {
          throw new FS.ErrnoError(28);
        }
  
        // destroy the nodes for this mount, and all its child mounts
        var node = lookup.node;
        var mount = node.mounted;
        var mounts = FS.getMounts(mount);
  
        Object.keys(FS.nameTable).forEach((hash) => {
          var current = FS.nameTable[hash];
  
          while (current) {
            var next = current.name_next;
  
            if (mounts.includes(current.mount)) {
              FS.destroyNode(current);
            }
  
            current = next;
          }
        });
  
        // no longer a mountpoint
        node.mounted = null;
  
        // remove this mount from the child mounts
        var idx = node.mount.mounts.indexOf(mount);
        assert(idx !== -1);
        node.mount.mounts.splice(idx, 1);
      },
  lookup(parent, name) {
        return parent.node_ops.lookup(parent, name);
      },
  mknod(path, mode, dev) {
        var lookup = FS.lookupPath(path, { parent: true });
        var parent = lookup.node;
        var name = PATH.basename(path);
        if (!name) {
          throw new FS.ErrnoError(28);
        }
        if (name === '.' || name === '..') {
          throw new FS.ErrnoError(20);
        }
        var errCode = FS.mayCreate(parent, name);
        if (errCode) {
          throw new FS.ErrnoError(errCode);
        }
        if (!parent.node_ops.mknod) {
          throw new FS.ErrnoError(63);
        }
        return parent.node_ops.mknod(parent, name, mode, dev);
      },
  statfs(path) {
        return FS.statfsNode(FS.lookupPath(path, {follow: true}).node);
      },
  statfsStream(stream) {
        // We keep a separate statfsStream function because noderawfs overrides
        // it. In noderawfs, stream.node is sometimes null. Instead, we need to
        // look at stream.path.
        return FS.statfsNode(stream.node);
      },
  statfsNode(node) {
        // NOTE: None of the defaults here are true. We're just returning safe and
        //       sane values. Currently nodefs and rawfs replace these defaults,
        //       other file systems leave them alone.
        var rtn = {
          bsize: 4096,
          frsize: 4096,
          blocks: 1e6,
          bfree: 5e5,
          bavail: 5e5,
          files: FS.nextInode,
          ffree: FS.nextInode - 1,
          fsid: 42,
          flags: 2,
          namelen: 255,
        };
  
        if (node.node_ops.statfs) {
          Object.assign(rtn, node.node_ops.statfs(node.mount.opts.root));
        }
        return rtn;
      },
  create(path, mode = 0o666) {
        mode &= 4095;
        mode |= 32768;
        return FS.mknod(path, mode, 0);
      },
  mkdir(path, mode = 0o777) {
        mode &= 511 | 512;
        mode |= 16384;
        return FS.mknod(path, mode, 0);
      },
  mkdirTree(path, mode) {
        var dirs = path.split('/');
        var d = '';
        for (var i = 0; i < dirs.length; ++i) {
          if (!dirs[i]) continue;
          d += '/' + dirs[i];
          try {
            FS.mkdir(d, mode);
          } catch(e) {
            if (e.errno != 20) throw e;
          }
        }
      },
  mkdev(path, mode, dev) {
        if (typeof dev == 'undefined') {
          dev = mode;
          mode = 0o666;
        }
        mode |= 8192;
        return FS.mknod(path, mode, dev);
      },
  symlink(oldpath, newpath) {
        if (!PATH_FS.resolve(oldpath)) {
          throw new FS.ErrnoError(44);
        }
        var lookup = FS.lookupPath(newpath, { parent: true });
        var parent = lookup.node;
        if (!parent) {
          throw new FS.ErrnoError(44);
        }
        var newname = PATH.basename(newpath);
        var errCode = FS.mayCreate(parent, newname);
        if (errCode) {
          throw new FS.ErrnoError(errCode);
        }
        if (!parent.node_ops.symlink) {
          throw new FS.ErrnoError(63);
        }
        return parent.node_ops.symlink(parent, newname, oldpath);
      },
  rename(old_path, new_path) {
        var old_dirname = PATH.dirname(old_path);
        var new_dirname = PATH.dirname(new_path);
        var old_name = PATH.basename(old_path);
        var new_name = PATH.basename(new_path);
        // parents must exist
        var lookup, old_dir, new_dir;
  
        // let the errors from non existent directories percolate up
        lookup = FS.lookupPath(old_path, { parent: true });
        old_dir = lookup.node;
        lookup = FS.lookupPath(new_path, { parent: true });
        new_dir = lookup.node;
  
        if (!old_dir || !new_dir) throw new FS.ErrnoError(44);
        // need to be part of the same mount
        if (old_dir.mount !== new_dir.mount) {
          throw new FS.ErrnoError(75);
        }
        // source must exist
        var old_node = FS.lookupNode(old_dir, old_name);
        // old path should not be an ancestor of the new path
        var relative = PATH_FS.relative(old_path, new_dirname);
        if (relative.charAt(0) !== '.') {
          throw new FS.ErrnoError(28);
        }
        // new path should not be an ancestor of the old path
        relative = PATH_FS.relative(new_path, old_dirname);
        if (relative.charAt(0) !== '.') {
          throw new FS.ErrnoError(55);
        }
        // see if the new path already exists
        var new_node;
        try {
          new_node = FS.lookupNode(new_dir, new_name);
        } catch (e) {
          // not fatal
        }
        // early out if nothing needs to change
        if (old_node === new_node) {
          return;
        }
        // we'll need to delete the old entry
        var isdir = FS.isDir(old_node.mode);
        var errCode = FS.mayDelete(old_dir, old_name, isdir);
        if (errCode) {
          throw new FS.ErrnoError(errCode);
        }
        // need delete permissions if we'll be overwriting.
        // need create permissions if new doesn't already exist.
        errCode = new_node ?
          FS.mayDelete(new_dir, new_name, isdir) :
          FS.mayCreate(new_dir, new_name);
        if (errCode) {
          throw new FS.ErrnoError(errCode);
        }
        if (!old_dir.node_ops.rename) {
          throw new FS.ErrnoError(63);
        }
        if (FS.isMountpoint(old_node) || (new_node && FS.isMountpoint(new_node))) {
          throw new FS.ErrnoError(10);
        }
        // if we are going to change the parent, check write permissions
        if (new_dir !== old_dir) {
          errCode = FS.nodePermissions(old_dir, 'w');
          if (errCode) {
            throw new FS.ErrnoError(errCode);
          }
        }
        // remove the node from the lookup hash
        FS.hashRemoveNode(old_node);
        // do the underlying fs rename
        try {
          old_dir.node_ops.rename(old_node, new_dir, new_name);
          // update old node (we do this here to avoid each backend
          // needing to)
          old_node.parent = new_dir;
        } catch (e) {
          throw e;
        } finally {
          // add the node back to the hash (in case node_ops.rename
          // changed its name)
          FS.hashAddNode(old_node);
        }
      },
  rmdir(path) {
        var lookup = FS.lookupPath(path, { parent: true });
        var parent = lookup.node;
        var name = PATH.basename(path);
        var node = FS.lookupNode(parent, name);
        var errCode = FS.mayDelete(parent, name, true);
        if (errCode) {
          throw new FS.ErrnoError(errCode);
        }
        if (!parent.node_ops.rmdir) {
          throw new FS.ErrnoError(63);
        }
        if (FS.isMountpoint(node)) {
          throw new FS.ErrnoError(10);
        }
        parent.node_ops.rmdir(parent, name);
        FS.destroyNode(node);
      },
  readdir(path) {
        var lookup = FS.lookupPath(path, { follow: true });
        var node = lookup.node;
        var readdir = FS.checkOpExists(node.node_ops.readdir, 54);
        return readdir(node);
      },
  unlink(path) {
        var lookup = FS.lookupPath(path, { parent: true });
        var parent = lookup.node;
        if (!parent) {
          throw new FS.ErrnoError(44);
        }
        var name = PATH.basename(path);
        var node = FS.lookupNode(parent, name);
        var errCode = FS.mayDelete(parent, name, false);
        if (errCode) {
          // According to POSIX, we should map EISDIR to EPERM, but
          // we instead do what Linux does (and we must, as we use
          // the musl linux libc).
          throw new FS.ErrnoError(errCode);
        }
        if (!parent.node_ops.unlink) {
          throw new FS.ErrnoError(63);
        }
        if (FS.isMountpoint(node)) {
          throw new FS.ErrnoError(10);
        }
        parent.node_ops.unlink(parent, name);
        FS.destroyNode(node);
      },
  readlink(path) {
        var lookup = FS.lookupPath(path);
        var link = lookup.node;
        if (!link) {
          throw new FS.ErrnoError(44);
        }
        if (!link.node_ops.readlink) {
          throw new FS.ErrnoError(28);
        }
        return link.node_ops.readlink(link);
      },
  stat(path, dontFollow) {
        var lookup = FS.lookupPath(path, { follow: !dontFollow });
        var node = lookup.node;
        var getattr = FS.checkOpExists(node.node_ops.getattr, 63);
        return getattr(node);
      },
  lstat(path) {
        return FS.stat(path, true);
      },
  chmod(path, mode, dontFollow) {
        var node;
        if (typeof path == 'string') {
          var lookup = FS.lookupPath(path, { follow: !dontFollow });
          node = lookup.node;
        } else {
          node = path;
        }
        var setattr = FS.checkOpExists(node.node_ops.setattr, 63);
        setattr(node, {
          mode: (mode & 4095) | (node.mode & ~4095),
          ctime: Date.now(),
          dontFollow
        });
      },
  lchmod(path, mode) {
        FS.chmod(path, mode, true);
      },
  fchmod(fd, mode) {
        var stream = FS.getStreamChecked(fd);
        FS.chmod(stream.node, mode);
      },
  chown(path, uid, gid, dontFollow) {
        var node;
        if (typeof path == 'string') {
          var lookup = FS.lookupPath(path, { follow: !dontFollow });
          node = lookup.node;
        } else {
          node = path;
        }
        var setattr = FS.checkOpExists(node.node_ops.setattr, 63);
        setattr(node, {
          timestamp: Date.now(),
          dontFollow
          // we ignore the uid / gid for now
        });
      },
  lchown(path, uid, gid) {
        FS.chown(path, uid, gid, true);
      },
  fchown(fd, uid, gid) {
        var stream = FS.getStreamChecked(fd);
        FS.chown(stream.node, uid, gid);
      },
  truncate(path, len) {
        if (len < 0) {
          throw new FS.ErrnoError(28);
        }
        var node;
        if (typeof path == 'string') {
          var lookup = FS.lookupPath(path, { follow: true });
          node = lookup.node;
        } else {
          node = path;
        }
        if (FS.isDir(node.mode)) {
          throw new FS.ErrnoError(31);
        }
        if (!FS.isFile(node.mode)) {
          throw new FS.ErrnoError(28);
        }
        var errCode = FS.nodePermissions(node, 'w');
        if (errCode) {
          throw new FS.ErrnoError(errCode);
        }
        var setattr = FS.checkOpExists(node.node_ops.setattr, 63);
        setattr(node, {
          size: len,
          timestamp: Date.now()
        });
      },
  ftruncate(fd, len) {
        var stream = FS.getStreamChecked(fd);
        if ((stream.flags & 2097155) === 0) {
          throw new FS.ErrnoError(28);
        }
        FS.truncate(stream.node, len);
      },
  utime(path, atime, mtime) {
        var lookup = FS.lookupPath(path, { follow: true });
        var node = lookup.node;
        var setattr = FS.checkOpExists(node.node_ops.setattr, 63);
        setattr(node, {
          atime: atime,
          mtime: mtime
        });
      },
  open(path, flags, mode = 0o666) {
        if (path === "") {
          throw new FS.ErrnoError(44);
        }
        flags = typeof flags == 'string' ? FS_modeStringToFlags(flags) : flags;
        if ((flags & 64)) {
          mode = (mode & 4095) | 32768;
        } else {
          mode = 0;
        }
        var node;
        var isDirPath;
        if (typeof path == 'object') {
          node = path;
        } else {
          isDirPath = path.endsWith("/");
          // noent_okay makes it so that if the final component of the path
          // doesn't exist, lookupPath returns `node: undefined`. `path` will be
          // updated to point to the target of all symlinks.
          var lookup = FS.lookupPath(path, {
            follow: !(flags & 131072),
            noent_okay: true
          });
          node = lookup.node;
          path = lookup.path;
        }
        // perhaps we need to create the node
        var created = false;
        if ((flags & 64)) {
          if (node) {
            // if O_CREAT and O_EXCL are set, error out if the node already exists
            if ((flags & 128)) {
              throw new FS.ErrnoError(20);
            }
          } else if (isDirPath) {
            throw new FS.ErrnoError(31);
          } else {
            // node doesn't exist, try to create it
            // Ignore the permission bits here to ensure we can `open` this new
            // file below. We use chmod below the apply the permissions once the
            // file is open.
            node = FS.mknod(path, mode | 0o777, 0);
            created = true;
          }
        }
        if (!node) {
          throw new FS.ErrnoError(44);
        }
        // can't truncate a device
        if (FS.isChrdev(node.mode)) {
          flags &= ~512;
        }
        // if asked only for a directory, then this must be one
        if ((flags & 65536) && !FS.isDir(node.mode)) {
          throw new FS.ErrnoError(54);
        }
        // check permissions, if this is not a file we just created now (it is ok to
        // create and write to a file with read-only permissions; it is read-only
        // for later use)
        if (!created) {
          var errCode = FS.mayOpen(node, flags);
          if (errCode) {
            throw new FS.ErrnoError(errCode);
          }
        }
        // do truncation if necessary
        if ((flags & 512) && !created) {
          FS.truncate(node, 0);
        }
        // we've already handled these, don't pass down to the underlying vfs
        flags &= ~(128 | 512 | 131072);
  
        // register the stream with the filesystem
        var stream = FS.createStream({
          node,
          path: FS.getPath(node),  // we want the absolute path to the node
          flags,
          seekable: true,
          position: 0,
          stream_ops: node.stream_ops,
          // used by the file family libc calls (fopen, fwrite, ferror, etc.)
          ungotten: [],
          error: false
        });
        // call the new stream's open function
        if (stream.stream_ops.open) {
          stream.stream_ops.open(stream);
        }
        if (created) {
          FS.chmod(node, mode & 0o777);
        }
        if (Module['logReadFiles'] && !(flags & 1)) {
          if (!(path in FS.readFiles)) {
            FS.readFiles[path] = 1;
          }
        }
        return stream;
      },
  close(stream) {
        if (FS.isClosed(stream)) {
          throw new FS.ErrnoError(8);
        }
        if (stream.getdents) stream.getdents = null; // free readdir state
        try {
          if (stream.stream_ops.close) {
            stream.stream_ops.close(stream);
          }
        } catch (e) {
          throw e;
        } finally {
          FS.closeStream(stream.fd);
        }
        stream.fd = null;
      },
  isClosed(stream) {
        return stream.fd === null;
      },
  llseek(stream, offset, whence) {
        if (FS.isClosed(stream)) {
          throw new FS.ErrnoError(8);
        }
        if (!stream.seekable || !stream.stream_ops.llseek) {
          throw new FS.ErrnoError(70);
        }
        if (whence != 0 && whence != 1 && whence != 2) {
          throw new FS.ErrnoError(28);
        }
        stream.position = stream.stream_ops.llseek(stream, offset, whence);
        stream.ungotten = [];
        return stream.position;
      },
  read(stream, buffer, offset, length, position) {
        assert(offset >= 0);
        if (length < 0 || position < 0) {
          throw new FS.ErrnoError(28);
        }
        if (FS.isClosed(stream)) {
          throw new FS.ErrnoError(8);
        }
        if ((stream.flags & 2097155) === 1) {
          throw new FS.ErrnoError(8);
        }
        if (FS.isDir(stream.node.mode)) {
          throw new FS.ErrnoError(31);
        }
        if (!stream.stream_ops.read) {
          throw new FS.ErrnoError(28);
        }
        var seeking = typeof position != 'undefined';
        if (!seeking) {
          position = stream.position;
        } else if (!stream.seekable) {
          throw new FS.ErrnoError(70);
        }
        var bytesRead = stream.stream_ops.read(stream, buffer, offset, length, position);
        if (!seeking) stream.position += bytesRead;
        return bytesRead;
      },
  write(stream, buffer, offset, length, position, canOwn) {
        assert(offset >= 0);
        if (length < 0 || position < 0) {
          throw new FS.ErrnoError(28);
        }
        if (FS.isClosed(stream)) {
          throw new FS.ErrnoError(8);
        }
        if ((stream.flags & 2097155) === 0) {
          throw new FS.ErrnoError(8);
        }
        if (FS.isDir(stream.node.mode)) {
          throw new FS.ErrnoError(31);
        }
        if (!stream.stream_ops.write) {
          throw new FS.ErrnoError(28);
        }
        if (stream.seekable && stream.flags & 1024) {
          // seek to the end before writing in append mode
          FS.llseek(stream, 0, 2);
        }
        var seeking = typeof position != 'undefined';
        if (!seeking) {
          position = stream.position;
        } else if (!stream.seekable) {
          throw new FS.ErrnoError(70);
        }
        var bytesWritten = stream.stream_ops.write(stream, buffer, offset, length, position, canOwn);
        if (!seeking) stream.position += bytesWritten;
        return bytesWritten;
      },
  allocate(stream, offset, length) {
        if (FS.isClosed(stream)) {
          throw new FS.ErrnoError(8);
        }
        if (offset < 0 || length <= 0) {
          throw new FS.ErrnoError(28);
        }
        if ((stream.flags & 2097155) === 0) {
          throw new FS.ErrnoError(8);
        }
        if (!FS.isFile(stream.node.mode) && !FS.isDir(stream.node.mode)) {
          throw new FS.ErrnoError(43);
        }
        if (!stream.stream_ops.allocate) {
          throw new FS.ErrnoError(138);
        }
        stream.stream_ops.allocate(stream, offset, length);
      },
  mmap(stream, length, position, prot, flags) {
        // User requests writing to file (prot & PROT_WRITE != 0).
        // Checking if we have permissions to write to the file unless
        // MAP_PRIVATE flag is set. According to POSIX spec it is possible
        // to write to file opened in read-only mode with MAP_PRIVATE flag,
        // as all modifications will be visible only in the memory of
        // the current process.
        if ((prot & 2) !== 0
            && (flags & 2) === 0
            && (stream.flags & 2097155) !== 2) {
          throw new FS.ErrnoError(2);
        }
        if ((stream.flags & 2097155) === 1) {
          throw new FS.ErrnoError(2);
        }
        if (!stream.stream_ops.mmap) {
          throw new FS.ErrnoError(43);
        }
        if (!length) {
          throw new FS.ErrnoError(28);
        }
        return stream.stream_ops.mmap(stream, length, position, prot, flags);
      },
  msync(stream, buffer, offset, length, mmapFlags) {
        assert(offset >= 0);
        if (!stream.stream_ops.msync) {
          return 0;
        }
        return stream.stream_ops.msync(stream, buffer, offset, length, mmapFlags);
      },
  ioctl(stream, cmd, arg) {
        if (!stream.stream_ops.ioctl) {
          throw new FS.ErrnoError(59);
        }
        return stream.stream_ops.ioctl(stream, cmd, arg);
      },
  readFile(path, opts = {}) {
        opts.flags = opts.flags || 0;
        opts.encoding = opts.encoding || 'binary';
        if (opts.encoding !== 'utf8' && opts.encoding !== 'binary') {
          throw new Error(`Invalid encoding type "${opts.encoding}"`);
        }
        var ret;
        var stream = FS.open(path, opts.flags);
        var stat = FS.stat(path);
        var length = stat.size;
        var buf = new Uint8Array(length);
        FS.read(stream, buf, 0, length, 0);
        if (opts.encoding === 'utf8') {
          ret = UTF8ArrayToString(buf);
        } else if (opts.encoding === 'binary') {
          ret = buf;
        }
        FS.close(stream);
        return ret;
      },
  writeFile(path, data, opts = {}) {
        opts.flags = opts.flags || 577;
        var stream = FS.open(path, opts.flags, opts.mode);
        if (typeof data == 'string') {
          var buf = new Uint8Array(lengthBytesUTF8(data)+1);
          var actualNumBytes = stringToUTF8Array(data, buf, 0, buf.length);
          FS.write(stream, buf, 0, actualNumBytes, undefined, opts.canOwn);
        } else if (ArrayBuffer.isView(data)) {
          FS.write(stream, data, 0, data.byteLength, undefined, opts.canOwn);
        } else {
          throw new Error('Unsupported data type');
        }
        FS.close(stream);
      },
  cwd:() => FS.currentPath,
  chdir(path) {
        var lookup = FS.lookupPath(path, { follow: true });
        if (lookup.node === null) {
          throw new FS.ErrnoError(44);
        }
        if (!FS.isDir(lookup.node.mode)) {
          throw new FS.ErrnoError(54);
        }
        var errCode = FS.nodePermissions(lookup.node, 'x');
        if (errCode) {
          throw new FS.ErrnoError(errCode);
        }
        FS.currentPath = lookup.path;
      },
  createDefaultDirectories() {
        FS.mkdir('/tmp');
        FS.mkdir('/home');
        FS.mkdir('/home/web_user');
      },
  createDefaultDevices() {
        // create /dev
        FS.mkdir('/dev');
        // setup /dev/null
        FS.registerDevice(FS.makedev(1, 3), {
          read: () => 0,
          write: (stream, buffer, offset, length, pos) => length,
          llseek: () => 0,
        });
        FS.mkdev('/dev/null', FS.makedev(1, 3));
        // setup /dev/tty and /dev/tty1
        // stderr needs to print output using err() rather than out()
        // so we register a second tty just for it.
        TTY.register(FS.makedev(5, 0), TTY.default_tty_ops);
        TTY.register(FS.makedev(6, 0), TTY.default_tty1_ops);
        FS.mkdev('/dev/tty', FS.makedev(5, 0));
        FS.mkdev('/dev/tty1', FS.makedev(6, 0));
        // setup /dev/[u]random
        // use a buffer to avoid overhead of individual crypto calls per byte
        var randomBuffer = new Uint8Array(1024), randomLeft = 0;
        var randomByte = () => {
          if (randomLeft === 0) {
            randomFill(randomBuffer);
            randomLeft = randomBuffer.byteLength;
          }
          return randomBuffer[--randomLeft];
        };
        FS.createDevice('/dev', 'random', randomByte);
        FS.createDevice('/dev', 'urandom', randomByte);
        // we're not going to emulate the actual shm device,
        // just create the tmp dirs that reside in it commonly
        FS.mkdir('/dev/shm');
        FS.mkdir('/dev/shm/tmp');
      },
  createSpecialDirectories() {
        // create /proc/self/fd which allows /proc/self/fd/6 => readlink gives the
        // name of the stream for fd 6 (see test_unistd_ttyname)
        FS.mkdir('/proc');
        var proc_self = FS.mkdir('/proc/self');
        FS.mkdir('/proc/self/fd');
        FS.mount({
          mount() {
            var node = FS.createNode(proc_self, 'fd', 16895, 73);
            node.stream_ops = {
              llseek: MEMFS.stream_ops.llseek,
            };
            node.node_ops = {
              lookup(parent, name) {
                var fd = +name;
                var stream = FS.getStreamChecked(fd);
                var ret = {
                  parent: null,
                  mount: { mountpoint: 'fake' },
                  node_ops: { readlink: () => stream.path },
                  id: fd + 1,
                };
                ret.parent = ret; // make it look like a simple root node
                return ret;
              },
              readdir() {
                return Array.from(FS.streams.entries())
                  .filter(([k, v]) => v)
                  .map(([k, v]) => k.toString());
              }
            };
            return node;
          }
        }, {}, '/proc/self/fd');
      },
  createStandardStreams(input, output, error) {
        // TODO deprecate the old functionality of a single
        // input / output callback and that utilizes FS.createDevice
        // and instead require a unique set of stream ops
  
        // by default, we symlink the standard streams to the
        // default tty devices. however, if the standard streams
        // have been overwritten we create a unique device for
        // them instead.
        if (input) {
          FS.createDevice('/dev', 'stdin', input);
        } else {
          FS.symlink('/dev/tty', '/dev/stdin');
        }
        if (output) {
          FS.createDevice('/dev', 'stdout', null, output);
        } else {
          FS.symlink('/dev/tty', '/dev/stdout');
        }
        if (error) {
          FS.createDevice('/dev', 'stderr', null, error);
        } else {
          FS.symlink('/dev/tty1', '/dev/stderr');
        }
  
        // open default streams for the stdin, stdout and stderr devices
        var stdin = FS.open('/dev/stdin', 0);
        var stdout = FS.open('/dev/stdout', 1);
        var stderr = FS.open('/dev/stderr', 1);
        assert(stdin.fd === 0, `invalid handle for stdin (${stdin.fd})`);
        assert(stdout.fd === 1, `invalid handle for stdout (${stdout.fd})`);
        assert(stderr.fd === 2, `invalid handle for stderr (${stderr.fd})`);
      },
  staticInit() {
        FS.nameTable = new Array(4096);
  
        FS.mount(MEMFS, {}, '/');
  
        FS.createDefaultDirectories();
        FS.createDefaultDevices();
        FS.createSpecialDirectories();
  
        FS.filesystems = {
          'MEMFS': MEMFS,
        };
      },
  init(input, output, error) {
        assert(!FS.initialized, 'FS.init was previously called. If you want to initialize later with custom parameters, remove any earlier calls (note that one is automatically added to the generated code)');
        FS.initialized = true;
  
        // Allow Module.stdin etc. to provide defaults, if none explicitly passed to us here
        input ??= Module['stdin'];
        output ??= Module['stdout'];
        error ??= Module['stderr'];
  
        FS.createStandardStreams(input, output, error);
      },
  quit() {
        FS.initialized = false;
        // force-flush all streams, so we get musl std streams printed out
        _fflush(0);
        // close all of our streams
        for (var i = 0; i < FS.streams.length; i++) {
          var stream = FS.streams[i];
          if (!stream) {
            continue;
          }
          FS.close(stream);
        }
      },
  findObject(path, dontResolveLastLink) {
        var ret = FS.analyzePath(path, dontResolveLastLink);
        if (!ret.exists) {
          return null;
        }
        return ret.object;
      },
  analyzePath(path, dontResolveLastLink) {
        // operate from within the context of the symlink's target
        try {
          var lookup = FS.lookupPath(path, { follow: !dontResolveLastLink });
          path = lookup.path;
        } catch (e) {
        }
        var ret = {
          isRoot: false, exists: false, error: 0, name: null, path: null, object: null,
          parentExists: false, parentPath: null, parentObject: null
        };
        try {
          var lookup = FS.lookupPath(path, { parent: true });
          ret.parentExists = true;
          ret.parentPath = lookup.path;
          ret.parentObject = lookup.node;
          ret.name = PATH.basename(path);
          lookup = FS.lookupPath(path, { follow: !dontResolveLastLink });
          ret.exists = true;
          ret.path = lookup.path;
          ret.object = lookup.node;
          ret.name = lookup.node.name;
          ret.isRoot = lookup.path === '/';
        } catch (e) {
          ret.error = e.errno;
        };
        return ret;
      },
  createPath(parent, path, canRead, canWrite) {
        parent = typeof parent == 'string' ? parent : FS.getPath(parent);
        var parts = path.split('/').reverse();
        while (parts.length) {
          var part = parts.pop();
          if (!part) continue;
          var current = PATH.join2(parent, part);
          try {
            FS.mkdir(current);
          } catch (e) {
            // ignore EEXIST
          }
          parent = current;
        }
        return current;
      },
  createFile(parent, name, properties, canRead, canWrite) {
        var path = PATH.join2(typeof parent == 'string' ? parent : FS.getPath(parent), name);
        var mode = FS_getMode(canRead, canWrite);
        return FS.create(path, mode);
      },
  createDataFile(parent, name, data, canRead, canWrite, canOwn) {
        var path = name;
        if (parent) {
          parent = typeof parent == 'string' ? parent : FS.getPath(parent);
          path = name ? PATH.join2(parent, name) : parent;
        }
        var mode = FS_getMode(canRead, canWrite);
        var node = FS.create(path, mode);
        if (data) {
          if (typeof data == 'string') {
            var arr = new Array(data.length);
            for (var i = 0, len = data.length; i < len; ++i) arr[i] = data.charCodeAt(i);
            data = arr;
          }
          // make sure we can write to the file
          FS.chmod(node, mode | 146);
          var stream = FS.open(node, 577);
          FS.write(stream, data, 0, data.length, 0, canOwn);
          FS.close(stream);
          FS.chmod(node, mode);
        }
      },
  createDevice(parent, name, input, output) {
        var path = PATH.join2(typeof parent == 'string' ? parent : FS.getPath(parent), name);
        var mode = FS_getMode(!!input, !!output);
        FS.createDevice.major ??= 64;
        var dev = FS.makedev(FS.createDevice.major++, 0);
        // Create a fake device that a set of stream ops to emulate
        // the old behavior.
        FS.registerDevice(dev, {
          open(stream) {
            stream.seekable = false;
          },
          close(stream) {
            // flush any pending line data
            if (output?.buffer?.length) {
              output(10);
            }
          },
          read(stream, buffer, offset, length, pos /* ignored */) {
            var bytesRead = 0;
            for (var i = 0; i < length; i++) {
              var result;
              try {
                result = input();
              } catch (e) {
                throw new FS.ErrnoError(29);
              }
              if (result === undefined && bytesRead === 0) {
                throw new FS.ErrnoError(6);
              }
              if (result === null || result === undefined) break;
              bytesRead++;
              buffer[offset+i] = result;
            }
            if (bytesRead) {
              stream.node.atime = Date.now();
            }
            return bytesRead;
          },
          write(stream, buffer, offset, length, pos) {
            for (var i = 0; i < length; i++) {
              try {
                output(buffer[offset+i]);
              } catch (e) {
                throw new FS.ErrnoError(29);
              }
            }
            if (length) {
              stream.node.mtime = stream.node.ctime = Date.now();
            }
            return i;
          }
        });
        return FS.mkdev(path, mode, dev);
      },
  forceLoadFile(obj) {
        if (obj.isDevice || obj.isFolder || obj.link || obj.contents) return true;
        if (typeof XMLHttpRequest != 'undefined') {
          throw new Error("Lazy loading should have been performed (contents set) in createLazyFile, but it was not. Lazy loading only works in web workers. Use --embed-file or --preload-file in emcc on the main thread.");
        } else { // Command-line.
          try {
            obj.contents = readBinary(obj.url);
            obj.usedBytes = obj.contents.length;
          } catch (e) {
            throw new FS.ErrnoError(29);
          }
        }
      },
  createLazyFile(parent, name, url, canRead, canWrite) {
        // Lazy chunked Uint8Array (implements get and length from Uint8Array).
        // Actual getting is abstracted away for eventual reuse.
        class LazyUint8Array {
          lengthKnown = false;
          chunks = []; // Loaded chunks. Index is the chunk number
          get(idx) {
            if (idx > this.length-1 || idx < 0) {
              return undefined;
            }
            var chunkOffset = idx % this.chunkSize;
            var chunkNum = (idx / this.chunkSize)|0;
            return this.getter(chunkNum)[chunkOffset];
          }
          setDataGetter(getter) {
            this.getter = getter;
          }
          cacheLength() {
            // Find length
            var xhr = new XMLHttpRequest();
            xhr.open('HEAD', url, false);
            xhr.send(null);
            if (!(xhr.status >= 200 && xhr.status < 300 || xhr.status === 304)) throw new Error("Couldn't load " + url + ". Status: " + xhr.status);
            var datalength = Number(xhr.getResponseHeader("Content-length"));
            var header;
            var hasByteServing = (header = xhr.getResponseHeader("Accept-Ranges")) && header === "bytes";
            var usesGzip = (header = xhr.getResponseHeader("Content-Encoding")) && header === "gzip";
  
            var chunkSize = 1024*1024; // Chunk size in bytes
  
            if (!hasByteServing) chunkSize = datalength;
  
            // Function to get a range from the remote URL.
            var doXHR = (from, to) => {
              if (from > to) throw new Error("invalid range (" + from + ", " + to + ") or no bytes requested!");
              if (to > datalength-1) throw new Error("only " + datalength + " bytes available! programmer error!");
  
              // TODO: Use mozResponseArrayBuffer, responseStream, etc. if available.
              var xhr = new XMLHttpRequest();
              xhr.open('GET', url, false);
              if (datalength !== chunkSize) xhr.setRequestHeader("Range", "bytes=" + from + "-" + to);
  
              // Some hints to the browser that we want binary data.
              xhr.responseType = 'arraybuffer';
              if (xhr.overrideMimeType) {
                xhr.overrideMimeType('text/plain; charset=x-user-defined');
              }
  
              xhr.send(null);
              if (!(xhr.status >= 200 && xhr.status < 300 || xhr.status === 304)) throw new Error("Couldn't load " + url + ". Status: " + xhr.status);
              if (xhr.response !== undefined) {
                return new Uint8Array(/** @type{Array<number>} */(xhr.response || []));
              }
              return intArrayFromString(xhr.responseText || '', true);
            };
            var lazyArray = this;
            lazyArray.setDataGetter((chunkNum) => {
              var start = chunkNum * chunkSize;
              var end = (chunkNum+1) * chunkSize - 1; // including this byte
              end = Math.min(end, datalength-1); // if datalength-1 is selected, this is the last block
              if (typeof lazyArray.chunks[chunkNum] == 'undefined') {
                lazyArray.chunks[chunkNum] = doXHR(start, end);
              }
              if (typeof lazyArray.chunks[chunkNum] == 'undefined') throw new Error('doXHR failed!');
              return lazyArray.chunks[chunkNum];
            });
  
            if (usesGzip || !datalength) {
              // if the server uses gzip or doesn't supply the length, we have to download the whole file to get the (uncompressed) length
              chunkSize = datalength = 1; // this will force getter(0)/doXHR do download the whole file
              datalength = this.getter(0).length;
              chunkSize = datalength;
              out("LazyFiles on gzip forces download of the whole file when length is accessed");
            }
  
            this._length = datalength;
            this._chunkSize = chunkSize;
            this.lengthKnown = true;
          }
          get length() {
            if (!this.lengthKnown) {
              this.cacheLength();
            }
            return this._length;
          }
          get chunkSize() {
            if (!this.lengthKnown) {
              this.cacheLength();
            }
            return this._chunkSize;
          }
        }
  
        if (typeof XMLHttpRequest != 'undefined') {
          if (!ENVIRONMENT_IS_WORKER) throw 'Cannot do synchronous binary XHRs outside webworkers in modern browsers. Use --embed-file or --preload-file in emcc';
          var lazyArray = new LazyUint8Array();
          var properties = { isDevice: false, contents: lazyArray };
        } else {
          var properties = { isDevice: false, url: url };
        }
  
        var node = FS.createFile(parent, name, properties, canRead, canWrite);
        // This is a total hack, but I want to get this lazy file code out of the
        // core of MEMFS. If we want to keep this lazy file concept I feel it should
        // be its own thin LAZYFS proxying calls to MEMFS.
        if (properties.contents) {
          node.contents = properties.contents;
        } else if (properties.url) {
          node.contents = null;
          node.url = properties.url;
        }
        // Add a function that defers querying the file size until it is asked the first time.
        Object.defineProperties(node, {
          usedBytes: {
            get: function() { return this.contents.length; }
          }
        });
        // override each stream op with one that tries to force load the lazy file first
        var stream_ops = {};
        var keys = Object.keys(node.stream_ops);
        keys.forEach((key) => {
          var fn = node.stream_ops[key];
          stream_ops[key] = (...args) => {
            FS.forceLoadFile(node);
            return fn(...args);
          };
        });
        function writeChunks(stream, buffer, offset, length, position) {
          var contents = stream.node.contents;
          if (position >= contents.length)
            return 0;
          var size = Math.min(contents.length - position, length);
          assert(size >= 0);
          if (contents.slice) { // normal array
            for (var i = 0; i < size; i++) {
              buffer[offset + i] = contents[position + i];
            }
          } else {
            for (var i = 0; i < size; i++) { // LazyUint8Array from sync binary XHR
              buffer[offset + i] = contents.get(position + i);
            }
          }
          return size;
        }
        // use a custom read function
        stream_ops.read = (stream, buffer, offset, length, position) => {
          FS.forceLoadFile(node);
          return writeChunks(stream, buffer, offset, length, position)
        };
        // use a custom mmap function
        stream_ops.mmap = (stream, length, position, prot, flags) => {
          FS.forceLoadFile(node);
          var ptr = mmapAlloc(length);
          if (!ptr) {
            throw new FS.ErrnoError(48);
          }
          writeChunks(stream, HEAP8, ptr, length, position);
          return { ptr, allocated: true };
        };
        node.stream_ops = stream_ops;
        return node;
      },
  absolutePath() {
        abort('FS.absolutePath has been removed; use PATH_FS.resolve instead');
      },
  createFolder() {
        abort('FS.createFolder has been removed; use FS.mkdir instead');
      },
  createLink() {
        abort('FS.createLink has been removed; use FS.symlink instead');
      },
  joinPath() {
        abort('FS.joinPath has been removed; use PATH.join instead');
      },
  mmapAlloc() {
        abort('FS.mmapAlloc has been replaced by the top level function mmapAlloc');
      },
  standardizePath() {
        abort('FS.standardizePath has been removed; use PATH.normalize instead');
      },
  };
  
  var SYSCALLS = {
  DEFAULT_POLLMASK:5,
  calculateAt(dirfd, path, allowEmpty) {
        if (PATH.isAbs(path)) {
          return path;
        }
        // relative path
        var dir;
        if (dirfd === -100) {
          dir = FS.cwd();
        } else {
          var dirstream = SYSCALLS.getStreamFromFD(dirfd);
          dir = dirstream.path;
        }
        if (path.length == 0) {
          if (!allowEmpty) {
            throw new FS.ErrnoError(44);;
          }
          return dir;
        }
        return dir + '/' + path;
      },
  writeStat(buf, stat) {
        HEAP32[((buf)>>2)] = stat.dev;
        HEAP32[(((buf)+(4))>>2)] = stat.mode;
        HEAPU32[(((buf)+(8))>>2)] = stat.nlink;
        HEAP32[(((buf)+(12))>>2)] = stat.uid;
        HEAP32[(((buf)+(16))>>2)] = stat.gid;
        HEAP32[(((buf)+(20))>>2)] = stat.rdev;
        HEAP64[(((buf)+(24))>>3)] = BigInt(stat.size);
        HEAP32[(((buf)+(32))>>2)] = 4096;
        HEAP32[(((buf)+(36))>>2)] = stat.blocks;
        var atime = stat.atime.getTime();
        var mtime = stat.mtime.getTime();
        var ctime = stat.ctime.getTime();
        HEAP64[(((buf)+(40))>>3)] = BigInt(Math.floor(atime / 1000));
        HEAPU32[(((buf)+(48))>>2)] = (atime % 1000) * 1000 * 1000;
        HEAP64[(((buf)+(56))>>3)] = BigInt(Math.floor(mtime / 1000));
        HEAPU32[(((buf)+(64))>>2)] = (mtime % 1000) * 1000 * 1000;
        HEAP64[(((buf)+(72))>>3)] = BigInt(Math.floor(ctime / 1000));
        HEAPU32[(((buf)+(80))>>2)] = (ctime % 1000) * 1000 * 1000;
        HEAP64[(((buf)+(88))>>3)] = BigInt(stat.ino);
        return 0;
      },
  writeStatFs(buf, stats) {
        HEAP32[(((buf)+(4))>>2)] = stats.bsize;
        HEAP32[(((buf)+(40))>>2)] = stats.bsize;
        HEAP32[(((buf)+(8))>>2)] = stats.blocks;
        HEAP32[(((buf)+(12))>>2)] = stats.bfree;
        HEAP32[(((buf)+(16))>>2)] = stats.bavail;
        HEAP32[(((buf)+(20))>>2)] = stats.files;
        HEAP32[(((buf)+(24))>>2)] = stats.ffree;
        HEAP32[(((buf)+(28))>>2)] = stats.fsid;
        HEAP32[(((buf)+(44))>>2)] = stats.flags;  // ST_NOSUID
        HEAP32[(((buf)+(36))>>2)] = stats.namelen;
      },
  doMsync(addr, stream, len, flags, offset) {
        if (!FS.isFile(stream.node.mode)) {
          throw new FS.ErrnoError(43);
        }
        if (flags & 2) {
          // MAP_PRIVATE calls need not to be synced back to underlying fs
          return 0;
        }
        var buffer = HEAPU8.slice(addr, addr + len);
        FS.msync(stream, buffer, offset, len, flags);
      },
  getStreamFromFD(fd) {
        var stream = FS.getStreamChecked(fd);
        return stream;
      },
  varargs:undefined,
  getStr(ptr) {
        var ret = UTF8ToString(ptr);
        return ret;
      },
  };
  function ___syscall_fcntl64(fd, cmd, varargs) {
  SYSCALLS.varargs = varargs;
  try {
  
      var stream = SYSCALLS.getStreamFromFD(fd);
      switch (cmd) {
        case 0: {
          var arg = syscallGetVarargI();
          if (arg < 0) {
            return -28;
          }
          while (FS.streams[arg]) {
            arg++;
          }
          var newStream;
          newStream = FS.dupStream(stream, arg);
          return newStream.fd;
        }
        case 1:
        case 2:
          return 0;  // FD_CLOEXEC makes no sense for a single process.
        case 3:
          return stream.flags;
        case 4: {
          var arg = syscallGetVarargI();
          stream.flags |= arg;
          return 0;
        }
        case 12: {
          var arg = syscallGetVarargP();
          var offset = 0;
          // We're always unlocked.
          HEAP16[(((arg)+(offset))>>1)] = 2;
          return 0;
        }
        case 13:
        case 14:
          return 0; // Pretend that the locking is successful.
      }
      return -28;
    } catch (e) {
    if (typeof FS == 'undefined' || !(e.name === 'ErrnoError')) throw e;
    return -e.errno;
  }
  }

  
  function ___syscall_ioctl(fd, op, varargs) {
  SYSCALLS.varargs = varargs;
  try {
  
      var stream = SYSCALLS.getStreamFromFD(fd);
      switch (op) {
        case 21509: {
          if (!stream.tty) return -59;
          return 0;
        }
        case 21505: {
          if (!stream.tty) return -59;
          if (stream.tty.ops.ioctl_tcgets) {
            var termios = stream.tty.ops.ioctl_tcgets(stream);
            var argp = syscallGetVarargP();
            HEAP32[((argp)>>2)] = termios.c_iflag || 0;
            HEAP32[(((argp)+(4))>>2)] = termios.c_oflag || 0;
            HEAP32[(((argp)+(8))>>2)] = termios.c_cflag || 0;
            HEAP32[(((argp)+(12))>>2)] = termios.c_lflag || 0;
            for (var i = 0; i < 32; i++) {
              HEAP8[(argp + i)+(17)] = termios.c_cc[i] || 0;
            }
            return 0;
          }
          return 0;
        }
        case 21510:
        case 21511:
        case 21512: {
          if (!stream.tty) return -59;
          return 0; // no-op, not actually adjusting terminal settings
        }
        case 21506:
        case 21507:
        case 21508: {
          if (!stream.tty) return -59;
          if (stream.tty.ops.ioctl_tcsets) {
            var argp = syscallGetVarargP();
            var c_iflag = HEAP32[((argp)>>2)];
            var c_oflag = HEAP32[(((argp)+(4))>>2)];
            var c_cflag = HEAP32[(((argp)+(8))>>2)];
            var c_lflag = HEAP32[(((argp)+(12))>>2)];
            var c_cc = []
            for (var i = 0; i < 32; i++) {
              c_cc.push(HEAP8[(argp + i)+(17)]);
            }
            return stream.tty.ops.ioctl_tcsets(stream.tty, op, { c_iflag, c_oflag, c_cflag, c_lflag, c_cc });
          }
          return 0; // no-op, not actually adjusting terminal settings
        }
        case 21519: {
          if (!stream.tty) return -59;
          var argp = syscallGetVarargP();
          HEAP32[((argp)>>2)] = 0;
          return 0;
        }
        case 21520: {
          if (!stream.tty) return -59;
          return -28; // not supported
        }
        case 21531: {
          var argp = syscallGetVarargP();
          return FS.ioctl(stream, op, argp);
        }
        case 21523: {
          // TODO: in theory we should write to the winsize struct that gets
          // passed in, but for now musl doesn't read anything on it
          if (!stream.tty) return -59;
          if (stream.tty.ops.ioctl_tiocgwinsz) {
            var winsize = stream.tty.ops.ioctl_tiocgwinsz(stream.tty);
            var argp = syscallGetVarargP();
            HEAP16[((argp)>>1)] = winsize[0];
            HEAP16[(((argp)+(2))>>1)] = winsize[1];
          }
          return 0;
        }
        case 21524: {
          // TODO: technically, this ioctl call should change the window size.
          // but, since emscripten doesn't have any concept of a terminal window
          // yet, we'll just silently throw it away as we do TIOCGWINSZ
          if (!stream.tty) return -59;
          return 0;
        }
        case 21515: {
          if (!stream.tty) return -59;
          return 0;
        }
        default: return -28; // not supported
      }
    } catch (e) {
    if (typeof FS == 'undefined' || !(e.name === 'ErrnoError')) throw e;
    return -e.errno;
  }
  }

  
  function ___syscall_openat(dirfd, path, flags, varargs) {
  SYSCALLS.varargs = varargs;
  try {
  
      path = SYSCALLS.getStr(path);
      path = SYSCALLS.calculateAt(dirfd, path);
      var mode = varargs ? syscallGetVarargI() : 0;
      return FS.open(path, flags, mode).fd;
    } catch (e) {
    if (typeof FS == 'undefined' || !(e.name === 'ErrnoError')) throw e;
    return -e.errno;
  }
  }

  var __abort_js = () =>
      abort('native code called abort()');

  var _emscripten_get_now = () => performance.now();
  
  var _emscripten_date_now = () => Date.now();
  
  var nowIsMonotonic = 1;
  
  var checkWasiClock = (clock_id) => clock_id >= 0 && clock_id <= 3;
  
  var INT53_MAX = 9007199254740992;
  
  var INT53_MIN = -9007199254740992;
  var bigintToI53Checked = (num) => (num < INT53_MIN || num > INT53_MAX) ? NaN : Number(num);
  function _clock_time_get(clk_id, ignored_precision, ptime) {
    ignored_precision = bigintToI53Checked(ignored_precision);
  
    
      if (!checkWasiClock(clk_id)) {
        return 28;
      }
      var now;
      // all wasi clocks but realtime are monotonic
      if (clk_id === 0) {
        now = _emscripten_date_now();
      } else if (nowIsMonotonic) {
        now = _emscripten_get_now();
      } else {
        return 52;
      }
      // "now" is in ms, and wasi times are in ns.
      var nsec = Math.round(now * 1000 * 1000);
      HEAP64[((ptime)>>3)] = BigInt(nsec);
      return 0;
    ;
  }

  var JSEvents = {
  memcpy(target, src, size) {
        HEAP8.set(HEAP8.subarray(src, src + size), target);
      },
  removeAllEventListeners() {
        while (JSEvents.eventHandlers.length) {
          JSEvents._removeHandler(JSEvents.eventHandlers.length - 1);
        }
        JSEvents.deferredCalls = [];
      },
  inEventHandler:0,
  deferredCalls:[],
  deferCall(targetFunction, precedence, argsList) {
        function arraysHaveEqualContent(arrA, arrB) {
          if (arrA.length != arrB.length) return false;
  
          for (var i in arrA) {
            if (arrA[i] != arrB[i]) return false;
          }
          return true;
        }
        // Test if the given call was already queued, and if so, don't add it again.
        for (var call of JSEvents.deferredCalls) {
          if (call.targetFunction == targetFunction && arraysHaveEqualContent(call.argsList, argsList)) {
            return;
          }
        }
        JSEvents.deferredCalls.push({
          targetFunction,
          precedence,
          argsList
        });
  
        JSEvents.deferredCalls.sort((x,y) => x.precedence < y.precedence);
      },
  removeDeferredCalls(targetFunction) {
        JSEvents.deferredCalls = JSEvents.deferredCalls.filter((call) => call.targetFunction != targetFunction);
      },
  canPerformEventHandlerRequests() {
        if (navigator.userActivation) {
          // Verify against transient activation status from UserActivation API
          // whether it is possible to perform a request here without needing to defer. See
          // https://developer.mozilla.org/en-US/docs/Web/Security/User_activation#transient_activation
          // and https://caniuse.com/mdn-api_useractivation
          // At the time of writing, Firefox does not support this API: https://bugzilla.mozilla.org/show_bug.cgi?id=1791079
          return navigator.userActivation.isActive;
        }
  
        return JSEvents.inEventHandler && JSEvents.currentEventHandler.allowsDeferredCalls;
      },
  runDeferredCalls() {
        if (!JSEvents.canPerformEventHandlerRequests()) {
          return;
        }
        var deferredCalls = JSEvents.deferredCalls;
        JSEvents.deferredCalls = [];
        for (var call of deferredCalls) {
          call.targetFunction(...call.argsList);
        }
      },
  eventHandlers:[],
  removeAllHandlersOnTarget:(target, eventTypeString) => {
        for (var i = 0; i < JSEvents.eventHandlers.length; ++i) {
          if (JSEvents.eventHandlers[i].target == target &&
            (!eventTypeString || eventTypeString == JSEvents.eventHandlers[i].eventTypeString)) {
             JSEvents._removeHandler(i--);
           }
        }
      },
  _removeHandler(i) {
        var h = JSEvents.eventHandlers[i];
        h.target.removeEventListener(h.eventTypeString, h.eventListenerFunc, h.useCapture);
        JSEvents.eventHandlers.splice(i, 1);
      },
  registerOrRemoveHandler(eventHandler) {
        if (!eventHandler.target) {
          err('registerOrRemoveHandler: the target element for event handler registration does not exist, when processing the following event handler registration:');
          console.dir(eventHandler);
          return -4;
        }
        if (eventHandler.callbackfunc) {
          eventHandler.eventListenerFunc = function(event) {
            // Increment nesting count for the event handler.
            ++JSEvents.inEventHandler;
            JSEvents.currentEventHandler = eventHandler;
            // Process any old deferred calls the user has placed.
            JSEvents.runDeferredCalls();
            // Process the actual event, calls back to user C code handler.
            eventHandler.handlerFunc(event);
            // Process any new deferred calls that were placed right now from this event handler.
            JSEvents.runDeferredCalls();
            // Out of event handler - restore nesting count.
            --JSEvents.inEventHandler;
          };
  
          eventHandler.target.addEventListener(eventHandler.eventTypeString,
                                               eventHandler.eventListenerFunc,
                                               eventHandler.useCapture);
          JSEvents.eventHandlers.push(eventHandler);
        } else {
          for (var i = 0; i < JSEvents.eventHandlers.length; ++i) {
            if (JSEvents.eventHandlers[i].target == eventHandler.target
             && JSEvents.eventHandlers[i].eventTypeString == eventHandler.eventTypeString) {
               JSEvents._removeHandler(i--);
             }
          }
        }
        return 0;
      },
  getNodeNameForTarget(target) {
        if (!target) return '';
        if (target == window) return '#window';
        if (target == screen) return '#screen';
        return target?.nodeName || '';
      },
  fullscreenEnabled() {
        return document.fullscreenEnabled
        // Safari 13.0.3 on macOS Catalina 10.15.1 still ships with prefixed webkitFullscreenEnabled.
        // TODO: If Safari at some point ships with unprefixed version, update the version check above.
        || document.webkitFullscreenEnabled
         ;
      },
  };
  
  var maybeCStringToJsString = (cString) => {
      // "cString > 2" checks if the input is a number, and isn't of the special
      // values we accept here, EMSCRIPTEN_EVENT_TARGET_* (which map to 0, 1, 2).
      // In other words, if cString > 2 then it's a pointer to a valid place in
      // memory, and points to a C string.
      return cString > 2 ? UTF8ToString(cString) : cString;
    };
  
  /** @type {Object} */
  var specialHTMLTargets = [0, typeof document != 'undefined' ? document : 0, typeof window != 'undefined' ? window : 0];
  var findEventTarget = (target) => {
      target = maybeCStringToJsString(target);
      var domElement = specialHTMLTargets[target] || (typeof document != 'undefined' ? document.querySelector(target) : null);
      return domElement;
    };
  
  var getBoundingClientRect = (e) => specialHTMLTargets.indexOf(e) < 0 ? e.getBoundingClientRect() : {'left':0,'top':0};
  var _emscripten_get_element_css_size = (target, width, height) => {
      target = findEventTarget(target);
      if (!target) return -4;
  
      var rect = getBoundingClientRect(target);
      HEAPF64[((width)>>3)] = rect.width;
      HEAPF64[((height)>>3)] = rect.height;
  
      return 0;
    };

  
  var requestPointerLock = (target) => {
      if (target.requestPointerLock) {
        target.requestPointerLock();
      } else {
        // document.body is known to accept pointer lock, so use that to differentiate if the user passed a bad element,
        // or if the whole browser just doesn't support the feature.
        if (document.body.requestPointerLock
          ) {
          return -3;
        }
        return -1;
      }
      return 0;
    };
  
  var _emscripten_request_pointerlock = (target, deferUntilInEventHandler) => {
      target = findEventTarget(target);
      if (!target) return -4;
      if (!target.requestPointerLock
        ) {
        return -1;
      }
  
      // Queue this function call if we're not currently in an event handler and
      // the user saw it appropriate to do so.
      if (!JSEvents.canPerformEventHandlerRequests()) {
        if (deferUntilInEventHandler) {
          JSEvents.deferCall(requestPointerLock, 2 /* priority below fullscreen */, [target]);
          return 1;
        }
        return -2;
      }
  
      return requestPointerLock(target);
    };

  var getHeapMax = () =>
      HEAPU8.length;
  
  
  var abortOnCannotGrowMemory = (requestedSize) => {
      abort(`Cannot enlarge memory arrays to size ${requestedSize} bytes (OOM). Either (1) compile with -sINITIAL_MEMORY=X with X higher than the current value ${HEAP8.length}, (2) compile with -sALLOW_MEMORY_GROWTH which allows increasing the size at runtime, or (3) if you want malloc to return NULL (0) instead of this abort, compile with -sABORTING_MALLOC=0`);
    };
  var _emscripten_resize_heap = (requestedSize) => {
      var oldSize = HEAPU8.length;
      // With CAN_ADDRESS_2GB or MEMORY64, pointers are already unsigned.
      requestedSize >>>= 0;
      abortOnCannotGrowMemory(requestedSize);
    };

  
  var _emscripten_set_element_css_size = (target, width, height) => {
      target = findEventTarget(target);
      if (!target) return -4;
  
      target.style.width = width + "px";
      target.style.height = height + "px";
  
      return 0;
    };

  
  
  var stringToUTF8 = (str, outPtr, maxBytesToWrite) => {
      assert(typeof maxBytesToWrite == 'number', 'stringToUTF8(str, outPtr, maxBytesToWrite) is missing the third parameter that specifies the length of the output buffer!');
      return stringToUTF8Array(str, HEAPU8, outPtr, maxBytesToWrite);
    };
  
  
  var wasmTableMirror = [];
  
  /** @type {WebAssembly.Table} */
  var wasmTable;
  var getWasmTableEntry = (funcPtr) => {
      var func = wasmTableMirror[funcPtr];
      if (!func) {
        if (funcPtr >= wasmTableMirror.length) wasmTableMirror.length = funcPtr + 1;
        /** @suppress {checkTypes} */
        wasmTableMirror[funcPtr] = func = wasmTable.get(funcPtr);
      }
      /** @suppress {checkTypes} */
      assert(wasmTable.get(funcPtr) == func, 'JavaScript-side Wasm function table mirror is out of date!');
      return func;
    };
  var registerKeyEventCallback = (target, userData, useCapture, callbackfunc, eventTypeId, eventTypeString, targetThread) => {
      JSEvents.keyEvent ||= _malloc(160);
  
      var keyEventHandlerFunc = (e) => {
        assert(e);
  
        var keyEventData = JSEvents.keyEvent;
        HEAPF64[((keyEventData)>>3)] = e.timeStamp;
  
        var idx = ((keyEventData)>>2);
  
        HEAP32[idx + 2] = e.location;
        HEAP8[keyEventData + 12] = e.ctrlKey;
        HEAP8[keyEventData + 13] = e.shiftKey;
        HEAP8[keyEventData + 14] = e.altKey;
        HEAP8[keyEventData + 15] = e.metaKey;
        HEAP8[keyEventData + 16] = e.repeat;
        HEAP32[idx + 5] = e.charCode;
        HEAP32[idx + 6] = e.keyCode;
        HEAP32[idx + 7] = e.which;
        stringToUTF8(e.key || '', keyEventData + 32, 32);
        stringToUTF8(e.code || '', keyEventData + 64, 32);
        stringToUTF8(e.char || '', keyEventData + 96, 32);
        stringToUTF8(e.locale || '', keyEventData + 128, 32);
  
        if (getWasmTableEntry(callbackfunc)(eventTypeId, keyEventData, userData)) e.preventDefault();
      };
  
      var eventHandler = {
        target: findEventTarget(target),
        eventTypeString,
        callbackfunc,
        handlerFunc: keyEventHandlerFunc,
        useCapture
      };
      return JSEvents.registerOrRemoveHandler(eventHandler);
    };
  var _emscripten_set_keydown_callback_on_thread = (target, userData, useCapture, callbackfunc, targetThread) =>
      registerKeyEventCallback(target, userData, useCapture, callbackfunc, 2, "keydown", targetThread);

  var _emscripten_set_keyup_callback_on_thread = (target, userData, useCapture, callbackfunc, targetThread) =>
      registerKeyEventCallback(target, userData, useCapture, callbackfunc, 3, "keyup", targetThread);

  
  var handleException = (e) => {
      // Certain exception types we do not treat as errors since they are used for
      // internal control flow.
      // 1. ExitStatus, which is thrown by exit()
      // 2. "unwind", which is thrown by emscripten_unwind_to_js_event_loop() and others
      //    that wish to return to JS event loop.
      if (e instanceof ExitStatus || e == 'unwind') {
        return EXITSTATUS;
      }
      checkStackCookie();
      if (e instanceof WebAssembly.RuntimeError) {
        if (_emscripten_stack_get_current() <= 0) {
          err('Stack overflow detected.  You can try increasing -sSTACK_SIZE (currently set to 65536)');
        }
      }
      quit_(1, e);
    };
  
  
  var runtimeKeepaliveCounter = 0;
  var keepRuntimeAlive = () => noExitRuntime || runtimeKeepaliveCounter > 0;
  var _proc_exit = (code) => {
      EXITSTATUS = code;
      if (!keepRuntimeAlive()) {
        Module['onExit']?.(code);
        ABORT = true;
      }
      quit_(code, new ExitStatus(code));
    };
  
  
  /** @suppress {duplicate } */
  /** @param {boolean|number=} implicit */
  var exitJS = (status, implicit) => {
      EXITSTATUS = status;
  
      checkUnflushedContent();
  
      // if exit() was called explicitly, warn the user if the runtime isn't actually being shut down
      if (keepRuntimeAlive() && !implicit) {
        var msg = `program exited (with status: ${status}), but keepRuntimeAlive() is set (counter=${runtimeKeepaliveCounter}) due to an async operation, so halting execution but not exiting the runtime or preventing further async execution (you can use emscripten_force_exit, if you want to force a true shutdown)`;
        err(msg);
      }
  
      _proc_exit(status);
    };
  var _exit = exitJS;
  
  
  var maybeExit = () => {
      if (!keepRuntimeAlive()) {
        try {
          _exit(EXITSTATUS);
        } catch (e) {
          handleException(e);
        }
      }
    };
  var callUserCallback = (func) => {
      if (ABORT) {
        err('user callback triggered after runtime exited or application aborted.  Ignoring.');
        return;
      }
      try {
        func();
        maybeExit();
      } catch (e) {
        handleException(e);
      }
    };
  
  var _emscripten_set_main_loop_timing = (mode, value) => {
      MainLoop.timingMode = mode;
      MainLoop.timingValue = value;
  
      if (!MainLoop.func) {
        err('emscripten_set_main_loop_timing: Cannot set timing mode for main loop since a main loop does not exist! Call emscripten_set_main_loop first to set one up.');
        return 1; // Return non-zero on failure, can't set timing mode when there is no main loop.
      }
  
      if (!MainLoop.running) {
        
        MainLoop.running = true;
      }
      if (mode == 0) {
        MainLoop.scheduler = function MainLoop_scheduler_setTimeout() {
          var timeUntilNextTick = Math.max(0, MainLoop.tickStartTime + value - _emscripten_get_now())|0;
          setTimeout(MainLoop.runner, timeUntilNextTick); // doing this each time means that on exception, we stop
        };
        MainLoop.method = 'timeout';
      } else if (mode == 1) {
        MainLoop.scheduler = function MainLoop_scheduler_rAF() {
          MainLoop.requestAnimationFrame(MainLoop.runner);
        };
        MainLoop.method = 'rAF';
      } else if (mode == 2) {
        if (typeof MainLoop.setImmediate == 'undefined') {
          if (typeof setImmediate == 'undefined') {
            // Emulate setImmediate. (note: not a complete polyfill, we don't emulate clearImmediate() to keep code size to minimum, since not needed)
            var setImmediates = [];
            var emscriptenMainLoopMessageId = 'setimmediate';
            /** @param {Event} event */
            var MainLoop_setImmediate_messageHandler = (event) => {
              // When called in current thread or Worker, the main loop ID is structured slightly different to accommodate for --proxy-to-worker runtime listening to Worker events,
              // so check for both cases.
              if (event.data === emscriptenMainLoopMessageId || event.data.target === emscriptenMainLoopMessageId) {
                event.stopPropagation();
                setImmediates.shift()();
              }
            };
            addEventListener("message", MainLoop_setImmediate_messageHandler, true);
            MainLoop.setImmediate = /** @type{function(function(): ?, ...?): number} */((func) => {
              setImmediates.push(func);
              if (ENVIRONMENT_IS_WORKER) {
                Module['setImmediates'] ??= [];
                Module['setImmediates'].push(func);
                postMessage({target: emscriptenMainLoopMessageId}); // In --proxy-to-worker, route the message via proxyClient.js
              } else postMessage(emscriptenMainLoopMessageId, "*"); // On the main thread, can just send the message to itself.
            });
          } else {
            MainLoop.setImmediate = setImmediate;
          }
        }
        MainLoop.scheduler = function MainLoop_scheduler_setImmediate() {
          MainLoop.setImmediate(MainLoop.runner);
        };
        MainLoop.method = 'immediate';
      }
      return 0;
    };
  var MainLoop = {
  running:false,
  scheduler:null,
  method:"",
  currentlyRunningMainloop:0,
  func:null,
  arg:0,
  timingMode:0,
  timingValue:0,
  currentFrameNumber:0,
  queue:[],
  preMainLoop:[],
  postMainLoop:[],
  pause() {
        MainLoop.scheduler = null;
        // Incrementing this signals the previous main loop that it's now become old, and it must return.
        MainLoop.currentlyRunningMainloop++;
      },
  resume() {
        MainLoop.currentlyRunningMainloop++;
        var timingMode = MainLoop.timingMode;
        var timingValue = MainLoop.timingValue;
        var func = MainLoop.func;
        MainLoop.func = null;
        // do not set timing and call scheduler, we will do it on the next lines
        setMainLoop(func, 0, false, MainLoop.arg, true);
        _emscripten_set_main_loop_timing(timingMode, timingValue);
        MainLoop.scheduler();
      },
  updateStatus() {
        if (Module['setStatus']) {
          var message = Module['statusMessage'] || 'Please wait...';
          var remaining = MainLoop.remainingBlockers ?? 0;
          var expected = MainLoop.expectedBlockers ?? 0;
          if (remaining) {
            if (remaining < expected) {
              Module['setStatus'](`{message} ({expected - remaining}/{expected})`);
            } else {
              Module['setStatus'](message);
            }
          } else {
            Module['setStatus']('');
          }
        }
      },
  init() {
        Module['preMainLoop'] && MainLoop.preMainLoop.push(Module['preMainLoop']);
        Module['postMainLoop'] && MainLoop.postMainLoop.push(Module['postMainLoop']);
      },
  runIter(func) {
        if (ABORT) return;
        for (var pre of MainLoop.preMainLoop) {
          if (pre() === false) {
            return; // |return false| skips a frame
          }
        }
        callUserCallback(func);
        for (var post of MainLoop.postMainLoop) {
          post();
        }
        checkStackCookie();
      },
  nextRAF:0,
  fakeRequestAnimationFrame(func) {
        // try to keep 60fps between calls to here
        var now = Date.now();
        if (MainLoop.nextRAF === 0) {
          MainLoop.nextRAF = now + 1000/60;
        } else {
          while (now + 2 >= MainLoop.nextRAF) { // fudge a little, to avoid timer jitter causing us to do lots of delay:0
            MainLoop.nextRAF += 1000/60;
          }
        }
        var delay = Math.max(MainLoop.nextRAF - now, 0);
        setTimeout(func, delay);
      },
  requestAnimationFrame(func) {
        if (typeof requestAnimationFrame == 'function') {
          requestAnimationFrame(func);
          return;
        }
        var RAF = MainLoop.fakeRequestAnimationFrame;
        RAF(func);
      },
  };
  
  
  
  
    /**
     * @param {number=} arg
     * @param {boolean=} noSetTiming
     */
  var setMainLoop = (iterFunc, fps, simulateInfiniteLoop, arg, noSetTiming) => {
      assert(!MainLoop.func, 'emscripten_set_main_loop: there can only be one main loop function at once: call emscripten_cancel_main_loop to cancel the previous one before setting a new one with different parameters.');
      MainLoop.func = iterFunc;
      MainLoop.arg = arg;
  
      var thisMainLoopId = MainLoop.currentlyRunningMainloop;
      function checkIsRunning() {
        if (thisMainLoopId < MainLoop.currentlyRunningMainloop) {
          
          maybeExit();
          return false;
        }
        return true;
      }
  
      // We create the loop runner here but it is not actually running until
      // _emscripten_set_main_loop_timing is called (which might happen a
      // later time).  This member signifies that the current runner has not
      // yet been started so that we can call runtimeKeepalivePush when it
      // gets it timing set for the first time.
      MainLoop.running = false;
      MainLoop.runner = function MainLoop_runner() {
        if (ABORT) return;
        if (MainLoop.queue.length > 0) {
          var start = Date.now();
          var blocker = MainLoop.queue.shift();
          blocker.func(blocker.arg);
          if (MainLoop.remainingBlockers) {
            var remaining = MainLoop.remainingBlockers;
            var next = remaining%1 == 0 ? remaining-1 : Math.floor(remaining);
            if (blocker.counted) {
              MainLoop.remainingBlockers = next;
            } else {
              // not counted, but move the progress along a tiny bit
              next = next + 0.5; // do not steal all the next one's progress
              MainLoop.remainingBlockers = (8*remaining + next)/9;
            }
          }
          MainLoop.updateStatus();
  
          // catches pause/resume main loop from blocker execution
          if (!checkIsRunning()) return;
  
          setTimeout(MainLoop.runner, 0);
          return;
        }
  
        // catch pauses from non-main loop sources
        if (!checkIsRunning()) return;
  
        // Implement very basic swap interval control
        MainLoop.currentFrameNumber = MainLoop.currentFrameNumber + 1 | 0;
        if (MainLoop.timingMode == 1 && MainLoop.timingValue > 1 && MainLoop.currentFrameNumber % MainLoop.timingValue != 0) {
          // Not the scheduled time to render this frame - skip.
          MainLoop.scheduler();
          return;
        } else if (MainLoop.timingMode == 0) {
          MainLoop.tickStartTime = _emscripten_get_now();
        }
  
        if (MainLoop.method === 'timeout' && Module['ctx']) {
          warnOnce('Looks like you are rendering without using requestAnimationFrame for the main loop. You should use 0 for the frame rate in emscripten_set_main_loop in order to use requestAnimationFrame, as that can greatly improve your frame rates!');
          MainLoop.method = ''; // just warn once per call to set main loop
        }
  
        MainLoop.runIter(iterFunc);
  
        // catch pauses from the main loop itself
        if (!checkIsRunning()) return;
  
        MainLoop.scheduler();
      }
  
      if (!noSetTiming) {
        if (fps && fps > 0) {
          _emscripten_set_main_loop_timing(0, 1000.0 / fps);
        } else {
          // Do rAF by rendering each frame (no decimating)
          _emscripten_set_main_loop_timing(1, 1);
        }
  
        MainLoop.scheduler();
      }
  
      if (simulateInfiniteLoop) {
        throw 'unwind';
      }
    };
  
  var _emscripten_set_main_loop = (func, fps, simulateInfiniteLoop) => {
      var iterFunc = getWasmTableEntry(func);
      setMainLoop(iterFunc, fps, simulateInfiniteLoop);
    };

  
  
  
  var fillMouseEventData = (eventStruct, e, target) => {
      assert(eventStruct % 4 == 0);
      HEAPF64[((eventStruct)>>3)] = e.timeStamp;
      var idx = ((eventStruct)>>2);
      HEAP32[idx + 2] = e.screenX;
      HEAP32[idx + 3] = e.screenY;
      HEAP32[idx + 4] = e.clientX;
      HEAP32[idx + 5] = e.clientY;
      HEAP8[eventStruct + 24] = e.ctrlKey;
      HEAP8[eventStruct + 25] = e.shiftKey;
      HEAP8[eventStruct + 26] = e.altKey;
      HEAP8[eventStruct + 27] = e.metaKey;
      HEAP16[idx*2 + 14] = e.button;
      HEAP16[idx*2 + 15] = e.buttons;
  
      HEAP32[idx + 8] = e["movementX"]
        ;
  
      HEAP32[idx + 9] = e["movementY"]
        ;
  
      // Note: rect contains doubles (truncated to placate SAFE_HEAP, which is the same behaviour when writing to HEAP32 anyway)
      var rect = getBoundingClientRect(target);
      HEAP32[idx + 10] = e.clientX - (rect.left | 0);
      HEAP32[idx + 11] = e.clientY - (rect.top  | 0);
  
    };
  
  
  
  var registerMouseEventCallback = (target, userData, useCapture, callbackfunc, eventTypeId, eventTypeString, targetThread) => {
      JSEvents.mouseEvent ||= _malloc(64);
      target = findEventTarget(target);
  
      var mouseEventHandlerFunc = (e = event) => {
        // TODO: Make this access thread safe, or this could update live while app is reading it.
        fillMouseEventData(JSEvents.mouseEvent, e, target);
  
        if (getWasmTableEntry(callbackfunc)(eventTypeId, JSEvents.mouseEvent, userData)) e.preventDefault();
      };
  
      var eventHandler = {
        target,
        allowsDeferredCalls: eventTypeString != 'mousemove' && eventTypeString != 'mouseenter' && eventTypeString != 'mouseleave', // Mouse move events do not allow fullscreen/pointer lock requests to be handled in them!
        eventTypeString,
        callbackfunc,
        handlerFunc: mouseEventHandlerFunc,
        useCapture
      };
      return JSEvents.registerOrRemoveHandler(eventHandler);
    };
  var _emscripten_set_mousemove_callback_on_thread = (target, userData, useCapture, callbackfunc, targetThread) =>
      registerMouseEventCallback(target, userData, useCapture, callbackfunc, 8, "mousemove", targetThread);

  
  
  
  var registerUiEventCallback = (target, userData, useCapture, callbackfunc, eventTypeId, eventTypeString, targetThread) => {
      JSEvents.uiEvent ||= _malloc(36);
  
      target = findEventTarget(target);
  
      var uiEventHandlerFunc = (e = event) => {
        if (e.target != target) {
          // Never take ui events such as scroll via a 'bubbled' route, but always from the direct element that
          // was targeted. Otherwise e.g. if app logs a message in response to a page scroll, the Emscripten log
          // message box could cause to scroll, generating a new (bubbled) scroll message, causing a new log print,
          // causing a new scroll, etc..
          return;
        }
        var b = document.body; // Take document.body to a variable, Closure compiler does not outline access to it on its own.
        if (!b) {
          // During a page unload 'body' can be null, with "Cannot read property 'clientWidth' of null" being thrown
          return;
        }
        var uiEvent = JSEvents.uiEvent;
        HEAP32[((uiEvent)>>2)] = 0; // always zero for resize and scroll
        HEAP32[(((uiEvent)+(4))>>2)] = b.clientWidth;
        HEAP32[(((uiEvent)+(8))>>2)] = b.clientHeight;
        HEAP32[(((uiEvent)+(12))>>2)] = innerWidth;
        HEAP32[(((uiEvent)+(16))>>2)] = innerHeight;
        HEAP32[(((uiEvent)+(20))>>2)] = outerWidth;
        HEAP32[(((uiEvent)+(24))>>2)] = outerHeight;
        HEAP32[(((uiEvent)+(28))>>2)] = pageXOffset | 0; // scroll offsets are float
        HEAP32[(((uiEvent)+(32))>>2)] = pageYOffset | 0;
        if (getWasmTableEntry(callbackfunc)(eventTypeId, uiEvent, userData)) e.preventDefault();
      };
  
      var eventHandler = {
        target,
        eventTypeString,
        callbackfunc,
        handlerFunc: uiEventHandlerFunc,
        useCapture
      };
      return JSEvents.registerOrRemoveHandler(eventHandler);
    };
  var _emscripten_set_resize_callback_on_thread = (target, userData, useCapture, callbackfunc, targetThread) =>
      registerUiEventCallback(target, userData, useCapture, callbackfunc, 10, "resize", targetThread);

  
  
  
  
  
  var registerWheelEventCallback = (target, userData, useCapture, callbackfunc, eventTypeId, eventTypeString, targetThread) => {
      JSEvents.wheelEvent ||= _malloc(96);
  
      // The DOM Level 3 events spec event 'wheel'
      var wheelHandlerFunc = (e = event) => {
        var wheelEvent = JSEvents.wheelEvent;
        fillMouseEventData(wheelEvent, e, target);
        HEAPF64[(((wheelEvent)+(64))>>3)] = e["deltaX"];
        HEAPF64[(((wheelEvent)+(72))>>3)] = e["deltaY"];
        HEAPF64[(((wheelEvent)+(80))>>3)] = e["deltaZ"];
        HEAP32[(((wheelEvent)+(88))>>2)] = e["deltaMode"];
        if (getWasmTableEntry(callbackfunc)(eventTypeId, wheelEvent, userData)) e.preventDefault();
      };
  
      var eventHandler = {
        target,
        allowsDeferredCalls: true,
        eventTypeString,
        callbackfunc,
        handlerFunc: wheelHandlerFunc,
        useCapture
      };
      return JSEvents.registerOrRemoveHandler(eventHandler);
    };
  
  var _emscripten_set_wheel_callback_on_thread = (target, userData, useCapture, callbackfunc, targetThread) => {
      target = findEventTarget(target);
      if (!target) return -4;
      if (typeof target.onwheel != 'undefined') {
        return registerWheelEventCallback(target, userData, useCapture, callbackfunc, 9, "wheel", targetThread);
      } else {
        return -1;
      }
    };

  
  
  
  
  var stackAlloc = (sz) => __emscripten_stack_alloc(sz);
  var stringToUTF8OnStack = (str) => {
      var size = lengthBytesUTF8(str) + 1;
      var ret = stackAlloc(size);
      stringToUTF8(str, ret, size);
      return ret;
    };
  
  
  
  var WebGPU = {
  errorCallback:(callback, type, message, userdata) => {
        var sp = stackSave();
        var messagePtr = stringToUTF8OnStack(message);
        getWasmTableEntry(callback)(type, messagePtr, userdata);
        stackRestore(sp);
      },
  initManagers:() => {
        assert(!WebGPU.mgrDevice, 'initManagers already called');
  
        /** @constructor */
        function Manager() {
          this.objects = {};
          this.nextId = 1;
          this.create = function(object, wrapper = {}) {
            var id = this.nextId++;
            assert(typeof this.objects[id] == 'undefined');
            wrapper.refcount = 1;
            wrapper.object = object;
            this.objects[id] = wrapper;
            return id;
          };
          this.get = function(id) {
            if (!id) return undefined;
            var o = this.objects[id];
            assert(typeof o != "undefined");
            return o.object;
          };
          this.reference = function(id) {
            var o = this.objects[id];
            assert(typeof o != "undefined");
            o.refcount++;
          };
          this.release = function(id) {
            var o = this.objects[id];
            assert(typeof o != "undefined");
            assert(o.refcount > 0);
            o.refcount--;
            if (o.refcount <= 0) {
              delete this.objects[id];
            }
          };
        }
  
        WebGPU.mgrSurface = new Manager();
        WebGPU.mgrSwapChain = new Manager();
  
        WebGPU.mgrAdapter = new Manager();
        // TODO: Release() the device's default queue when the device is freed.
        WebGPU.mgrDevice = new Manager();
        WebGPU.mgrQueue = new Manager();
  
        WebGPU.mgrCommandBuffer = new Manager();
        WebGPU.mgrCommandEncoder = new Manager();
        WebGPU.mgrRenderPassEncoder = new Manager();
        WebGPU.mgrComputePassEncoder = new Manager();
  
        WebGPU.mgrBindGroup = new Manager();
        WebGPU.mgrBuffer = new Manager();
        WebGPU.mgrSampler = new Manager();
        WebGPU.mgrTexture = new Manager();
        WebGPU.mgrTextureView = new Manager();
        WebGPU.mgrQuerySet = new Manager();
  
        WebGPU.mgrBindGroupLayout = new Manager();
        WebGPU.mgrPipelineLayout = new Manager();
        WebGPU.mgrRenderPipeline = new Manager();
        WebGPU.mgrComputePipeline = new Manager();
        WebGPU.mgrShaderModule = new Manager();
  
        WebGPU.mgrRenderBundleEncoder = new Manager();
        WebGPU.mgrRenderBundle = new Manager();
      },
  makeColor:(ptr) => {
        return {
          "r": HEAPF64[((ptr)>>3)],
          "g": HEAPF64[(((ptr)+(8))>>3)],
          "b": HEAPF64[(((ptr)+(16))>>3)],
          "a": HEAPF64[(((ptr)+(24))>>3)],
        };
      },
  makeExtent3D:(ptr) => {
        return {
          "width": HEAPU32[((ptr)>>2)],
          "height": HEAPU32[(((ptr)+(4))>>2)],
          "depthOrArrayLayers": HEAPU32[(((ptr)+(8))>>2)],
        };
      },
  makeOrigin3D:(ptr) => {
        return {
          "x": HEAPU32[((ptr)>>2)],
          "y": HEAPU32[(((ptr)+(4))>>2)],
          "z": HEAPU32[(((ptr)+(8))>>2)],
        };
      },
  makeImageCopyTexture:(ptr) => {
        assert(ptr);assert(HEAPU32[((ptr)>>2)] === 0);
        return {
          "texture": WebGPU.mgrTexture.get(
            HEAPU32[(((ptr)+(4))>>2)]),
          "mipLevel": HEAPU32[(((ptr)+(8))>>2)],
          "origin": WebGPU.makeOrigin3D(ptr + 12),
          "aspect": WebGPU.TextureAspect[HEAPU32[(((ptr)+(24))>>2)]],
        };
      },
  makeTextureDataLayout:(ptr) => {
        assert(ptr);assert(HEAPU32[((ptr)>>2)] === 0);
        var bytesPerRow = HEAPU32[(((ptr)+(16))>>2)];
        var rowsPerImage = HEAPU32[(((ptr)+(20))>>2)];
        return {
          "offset": HEAPU32[((((ptr + 4))+(8))>>2)] * 0x100000000 + HEAPU32[(((ptr)+(8))>>2)],
          "bytesPerRow": bytesPerRow === 4294967295 ? undefined : bytesPerRow,
          "rowsPerImage": rowsPerImage === 4294967295 ? undefined : rowsPerImage,
        };
      },
  makeImageCopyBuffer:(ptr) => {
        assert(ptr);assert(HEAPU32[((ptr)>>2)] === 0);
        var layoutPtr = ptr + 8;
        var bufferCopyView = WebGPU.makeTextureDataLayout(layoutPtr);
        bufferCopyView["buffer"] = WebGPU.mgrBuffer.get(
          HEAPU32[(((ptr)+(32))>>2)]);
        return bufferCopyView;
      },
  makePipelineConstants:(constantCount, constantsPtr) => {
        if (!constantCount) return;
        var constants = {};
        for (var i = 0; i < constantCount; ++i) {
          var entryPtr = constantsPtr + 16 * i;
          var key = UTF8ToString(HEAPU32[(((entryPtr)+(4))>>2)]);
          constants[key] = HEAPF64[(((entryPtr)+(8))>>3)];
        }
        return constants;
      },
  makePipelineLayout:(layoutPtr) => {
        if (!layoutPtr) return 'auto';
        return WebGPU.mgrPipelineLayout.get(layoutPtr);
      },
  makeProgrammableStageDescriptor:(ptr) => {
        if (!ptr) return undefined;
        assert(ptr);assert(HEAPU32[((ptr)>>2)] === 0);
        var desc = {
          "module": WebGPU.mgrShaderModule.get(
            HEAPU32[(((ptr)+(4))>>2)]),
          "constants": WebGPU.makePipelineConstants(
            HEAPU32[(((ptr)+(12))>>2)],
            HEAPU32[(((ptr)+(16))>>2)]),
        };
        var entryPointPtr = HEAPU32[(((ptr)+(8))>>2)];
        if (entryPointPtr) desc["entryPoint"] = UTF8ToString(entryPointPtr);
        return desc;
      },
  fillLimitStruct:(limits, supportedLimitsOutPtr) => {
        var limitsOutPtr = supportedLimitsOutPtr + 8;
  
        function setLimitValueU32(name, limitOffset) {
          var limitValue = limits[name];
          HEAP32[(((limitsOutPtr)+(limitOffset))>>2)] = limitValue;
        }
        function setLimitValueU64(name, limitOffset) {
          var limitValue = limits[name];
          HEAP64[(((limitsOutPtr)+(limitOffset))>>3)] = BigInt(limitValue);
        }
    
        setLimitValueU32('maxTextureDimension1D', 0);
        setLimitValueU32('maxTextureDimension2D', 4);
        setLimitValueU32('maxTextureDimension3D', 8);
        setLimitValueU32('maxTextureArrayLayers', 12);
        setLimitValueU32('maxBindGroups', 16);
        setLimitValueU32('maxBindGroupsPlusVertexBuffers', 20);
        setLimitValueU32('maxBindingsPerBindGroup', 24);
        setLimitValueU32('maxDynamicUniformBuffersPerPipelineLayout', 28);
        setLimitValueU32('maxDynamicStorageBuffersPerPipelineLayout', 32);
        setLimitValueU32('maxSampledTexturesPerShaderStage', 36);
        setLimitValueU32('maxSamplersPerShaderStage', 40);
        setLimitValueU32('maxStorageBuffersPerShaderStage', 44);
        setLimitValueU32('maxStorageTexturesPerShaderStage', 48);
        setLimitValueU32('maxUniformBuffersPerShaderStage', 52);
        setLimitValueU32('minUniformBufferOffsetAlignment', 72);
        setLimitValueU32('minStorageBufferOffsetAlignment', 76);
    
        setLimitValueU64('maxUniformBufferBindingSize', 56);
        setLimitValueU64('maxStorageBufferBindingSize', 64);
    
        setLimitValueU32('maxVertexBuffers', 80);
        setLimitValueU64('maxBufferSize', 88);
        setLimitValueU32('maxVertexAttributes', 96);
        setLimitValueU32('maxVertexBufferArrayStride', 100);
        setLimitValueU32('maxInterStageShaderComponents', 104);
        setLimitValueU32('maxInterStageShaderVariables', 108);
        setLimitValueU32('maxColorAttachments', 112);
        setLimitValueU32('maxColorAttachmentBytesPerSample', 116);
        setLimitValueU32('maxComputeWorkgroupStorageSize', 120);
        setLimitValueU32('maxComputeInvocationsPerWorkgroup', 124);
        setLimitValueU32('maxComputeWorkgroupSizeX', 128);
        setLimitValueU32('maxComputeWorkgroupSizeY', 132);
        setLimitValueU32('maxComputeWorkgroupSizeZ', 136);
        setLimitValueU32('maxComputeWorkgroupsPerDimension', 140);
      },
  Int_BufferMapState:{
  unmapped:1,
  pending:2,
  mapped:3,
  },
  Int_CompilationMessageType:{
  error:1,
  warning:2,
  info:3,
  },
  Int_DeviceLostReason:{
  undefined:1,
  unknown:1,
  destroyed:2,
  },
  Int_PreferredFormat:{
  rgba8unorm:18,
  bgra8unorm:23,
  },
  WGSLFeatureName:[,"readonly_and_readwrite_storage_textures","packed_4x8_integer_dot_product","unrestricted_pointer_parameters","pointer_composite_access"],
  AddressMode:[,"clamp-to-edge","repeat","mirror-repeat"],
  AlphaMode:[,"opaque","premultiplied"],
  BlendFactor:[,"zero","one","src","one-minus-src","src-alpha","one-minus-src-alpha","dst","one-minus-dst","dst-alpha","one-minus-dst-alpha","src-alpha-saturated","constant","one-minus-constant"],
  BlendOperation:[,"add","subtract","reverse-subtract","min","max"],
  BufferBindingType:[,"uniform","storage","read-only-storage"],
  BufferMapState:{
  1:"unmapped",
  2:"pending",
  3:"mapped",
  },
  CompareFunction:[,"never","less","equal","less-equal","greater","not-equal","greater-equal","always"],
  CompilationInfoRequestStatus:["success","error","device-lost","unknown"],
  CullMode:[,"none","front","back"],
  ErrorFilter:{
  1:"validation",
  2:"out-of-memory",
  3:"internal",
  },
  FeatureName:[,"depth-clip-control","depth32float-stencil8","timestamp-query","texture-compression-bc","texture-compression-etc2","texture-compression-astc","indirect-first-instance","shader-f16","rg11b10ufloat-renderable","bgra8unorm-storage","float32-filterable"],
  FilterMode:[,"nearest","linear"],
  FrontFace:[,"ccw","cw"],
  IndexFormat:[,"uint16","uint32"],
  LoadOp:[,"clear","load"],
  MipmapFilterMode:[,"nearest","linear"],
  PowerPreference:[,"low-power","high-performance"],
  PrimitiveTopology:[,"point-list","line-list","line-strip","triangle-list","triangle-strip"],
  QueryType:{
  1:"occlusion",
  2:"timestamp",
  },
  SamplerBindingType:[,"filtering","non-filtering","comparison"],
  StencilOperation:[,"keep","zero","replace","invert","increment-clamp","decrement-clamp","increment-wrap","decrement-wrap"],
  StorageTextureAccess:[,"write-only","read-only","read-write"],
  StoreOp:[,"store","discard"],
  TextureAspect:[,"all","stencil-only","depth-only"],
  TextureDimension:[,"1d","2d","3d"],
  TextureFormat:[,"r8unorm","r8snorm","r8uint","r8sint","r16uint","r16sint","r16float","rg8unorm","rg8snorm","rg8uint","rg8sint","r32float","r32uint","r32sint","rg16uint","rg16sint","rg16float","rgba8unorm","rgba8unorm-srgb","rgba8snorm","rgba8uint","rgba8sint","bgra8unorm","bgra8unorm-srgb","rgb10a2uint","rgb10a2unorm","rg11b10ufloat","rgb9e5ufloat","rg32float","rg32uint","rg32sint","rgba16uint","rgba16sint","rgba16float","rgba32float","rgba32uint","rgba32sint","stencil8","depth16unorm","depth24plus","depth24plus-stencil8","depth32float","depth32float-stencil8","bc1-rgba-unorm","bc1-rgba-unorm-srgb","bc2-rgba-unorm","bc2-rgba-unorm-srgb","bc3-rgba-unorm","bc3-rgba-unorm-srgb","bc4-r-unorm","bc4-r-snorm","bc5-rg-unorm","bc5-rg-snorm","bc6h-rgb-ufloat","bc6h-rgb-float","bc7-rgba-unorm","bc7-rgba-unorm-srgb","etc2-rgb8unorm","etc2-rgb8unorm-srgb","etc2-rgb8a1unorm","etc2-rgb8a1unorm-srgb","etc2-rgba8unorm","etc2-rgba8unorm-srgb","eac-r11unorm","eac-r11snorm","eac-rg11unorm","eac-rg11snorm","astc-4x4-unorm","astc-4x4-unorm-srgb","astc-5x4-unorm","astc-5x4-unorm-srgb","astc-5x5-unorm","astc-5x5-unorm-srgb","astc-6x5-unorm","astc-6x5-unorm-srgb","astc-6x6-unorm","astc-6x6-unorm-srgb","astc-8x5-unorm","astc-8x5-unorm-srgb","astc-8x6-unorm","astc-8x6-unorm-srgb","astc-8x8-unorm","astc-8x8-unorm-srgb","astc-10x5-unorm","astc-10x5-unorm-srgb","astc-10x6-unorm","astc-10x6-unorm-srgb","astc-10x8-unorm","astc-10x8-unorm-srgb","astc-10x10-unorm","astc-10x10-unorm-srgb","astc-12x10-unorm","astc-12x10-unorm-srgb","astc-12x12-unorm","astc-12x12-unorm-srgb"],
  TextureSampleType:[,"float","unfilterable-float","depth","sint","uint"],
  TextureViewDimension:[,"1d","2d","2d-array","cube","cube-array","3d"],
  VertexFormat:[,"uint8x2","uint8x4","sint8x2","sint8x4","unorm8x2","unorm8x4","snorm8x2","snorm8x4","uint16x2","uint16x4","sint16x2","sint16x4","unorm16x2","unorm16x4","snorm16x2","snorm16x4","float16x2","float16x4","float32","float32x2","float32x3","float32x4","uint32","uint32x2","uint32x3","uint32x4","sint32","sint32x2","sint32x3","sint32x4","unorm10-10-10-2"],
  VertexStepMode:[,"vertex-buffer-not-used","vertex","instance"],
  FeatureNameString2Enum:{
  undefined:"0",
  'depth-clip-control':"1",
  'depth32float-stencil8':"2",
  'timestamp-query':"3",
  'texture-compression-bc':"4",
  'texture-compression-etc2':"5",
  'texture-compression-astc':"6",
  'indirect-first-instance':"7",
  'shader-f16':"8",
  'rg11b10ufloat-renderable':"9",
  'bgra8unorm-storage':"10",
  'float32-filterable':"11",
  },
  };
  var _emscripten_webgpu_get_device = () => {
      assert(Module['preinitializedWebGPUDevice']);
      if (WebGPU.preinitializedDeviceId === undefined) {
        var device = Module['preinitializedWebGPUDevice'];
        var deviceWrapper = { queueId: WebGPU.mgrQueue.create(device["queue"]) };
        WebGPU.preinitializedDeviceId = WebGPU.mgrDevice.create(device, deviceWrapper);
      }
      WebGPU.mgrDevice.reference(WebGPU.preinitializedDeviceId);
      return WebGPU.preinitializedDeviceId;
    };


  function _fd_close(fd) {
  try {
  
      var stream = SYSCALLS.getStreamFromFD(fd);
      FS.close(stream);
      return 0;
    } catch (e) {
    if (typeof FS == 'undefined' || !(e.name === 'ErrnoError')) throw e;
    return e.errno;
  }
  }

  /** @param {number=} offset */
  var doReadv = (stream, iov, iovcnt, offset) => {
      var ret = 0;
      for (var i = 0; i < iovcnt; i++) {
        var ptr = HEAPU32[((iov)>>2)];
        var len = HEAPU32[(((iov)+(4))>>2)];
        iov += 8;
        var curr = FS.read(stream, HEAP8, ptr, len, offset);
        if (curr < 0) return -1;
        ret += curr;
        if (curr < len) break; // nothing more to read
        if (typeof offset != 'undefined') {
          offset += curr;
        }
      }
      return ret;
    };
  
  function _fd_read(fd, iov, iovcnt, pnum) {
  try {
  
      var stream = SYSCALLS.getStreamFromFD(fd);
      var num = doReadv(stream, iov, iovcnt);
      HEAPU32[((pnum)>>2)] = num;
      return 0;
    } catch (e) {
    if (typeof FS == 'undefined' || !(e.name === 'ErrnoError')) throw e;
    return e.errno;
  }
  }

  
  function _fd_seek(fd, offset, whence, newOffset) {
    offset = bigintToI53Checked(offset);
  
    
  try {
  
      if (isNaN(offset)) return 61;
      var stream = SYSCALLS.getStreamFromFD(fd);
      FS.llseek(stream, offset, whence);
      HEAP64[((newOffset)>>3)] = BigInt(stream.position);
      if (stream.getdents && offset === 0 && whence === 0) stream.getdents = null; // reset readdir state
      return 0;
    } catch (e) {
    if (typeof FS == 'undefined' || !(e.name === 'ErrnoError')) throw e;
    return e.errno;
  }
  ;
  }

  /** @param {number=} offset */
  var doWritev = (stream, iov, iovcnt, offset) => {
      var ret = 0;
      for (var i = 0; i < iovcnt; i++) {
        var ptr = HEAPU32[((iov)>>2)];
        var len = HEAPU32[(((iov)+(4))>>2)];
        iov += 8;
        var curr = FS.write(stream, HEAP8, ptr, len, offset);
        if (curr < 0) return -1;
        ret += curr;
        if (curr < len) {
          // No more space to write.
          break;
        }
        if (typeof offset != 'undefined') {
          offset += curr;
        }
      }
      return ret;
    };
  
  function _fd_write(fd, iov, iovcnt, pnum) {
  try {
  
      var stream = SYSCALLS.getStreamFromFD(fd);
      var num = doWritev(stream, iov, iovcnt);
      HEAPU32[((pnum)>>2)] = num;
      return 0;
    } catch (e) {
    if (typeof FS == 'undefined' || !(e.name === 'ErrnoError')) throw e;
    return e.errno;
  }
  }

  var _wgpuBindGroupLayoutRelease = (id) => WebGPU.mgrBindGroupLayout.release(id);

  var _wgpuCommandBufferRelease = (id) => WebGPU.mgrCommandBuffer.release(id);

  
  var _wgpuCommandEncoderBeginRenderPass = (encoderId, descriptor) => {
      assert(descriptor);
  
      function makeColorAttachment(caPtr) {
        var viewPtr = HEAPU32[(((caPtr)+(4))>>2)];
        if (viewPtr === 0) {
          // view could be undefined.
          return undefined;
        }
  
        var depthSlice = HEAP32[(((caPtr)+(8))>>2)];
        if (depthSlice == -1) depthSlice = undefined;
  
        var loadOpInt = HEAPU32[(((caPtr)+(16))>>2)];
            assert(loadOpInt !== 0);
  
        var storeOpInt = HEAPU32[(((caPtr)+(20))>>2)];
            assert(storeOpInt !== 0);
  
        var clearValue = WebGPU.makeColor(caPtr + 24);
  
        return {
          "view": WebGPU.mgrTextureView.get(viewPtr),
          "depthSlice": depthSlice,
          "resolveTarget": WebGPU.mgrTextureView.get(
            HEAPU32[(((caPtr)+(12))>>2)]),
          "clearValue": clearValue,
          "loadOp":  WebGPU.LoadOp[loadOpInt],
          "storeOp": WebGPU.StoreOp[storeOpInt],
        };
      }
  
      function makeColorAttachments(count, caPtr) {
        var attachments = [];
        for (var i = 0; i < count; ++i) {
          attachments.push(makeColorAttachment(caPtr + 56 * i));
        }
        return attachments;
      }
  
      function makeDepthStencilAttachment(dsaPtr) {
        if (dsaPtr === 0) return undefined;
  
        return {
          "view": WebGPU.mgrTextureView.get(
            HEAPU32[((dsaPtr)>>2)]),
          "depthClearValue": HEAPF32[(((dsaPtr)+(12))>>2)],
          "depthLoadOp": WebGPU.LoadOp[
            HEAPU32[(((dsaPtr)+(4))>>2)]],
          "depthStoreOp": WebGPU.StoreOp[
            HEAPU32[(((dsaPtr)+(8))>>2)]],
          "depthReadOnly": !!(HEAPU32[(((dsaPtr)+(16))>>2)]),
          "stencilClearValue": HEAPU32[(((dsaPtr)+(28))>>2)],
          "stencilLoadOp": WebGPU.LoadOp[
            HEAPU32[(((dsaPtr)+(20))>>2)]],
          "stencilStoreOp": WebGPU.StoreOp[
            HEAPU32[(((dsaPtr)+(24))>>2)]],
          "stencilReadOnly": !!(HEAPU32[(((dsaPtr)+(32))>>2)]),
        };
      }
  
      function makeRenderPassTimestampWrites(twPtr) {
        if (twPtr === 0) return undefined;
  
        return {
          "querySet": WebGPU.mgrQuerySet.get(
            HEAPU32[((twPtr)>>2)]),
          "beginningOfPassWriteIndex": HEAPU32[(((twPtr)+(4))>>2)],
          "endOfPassWriteIndex": HEAPU32[(((twPtr)+(8))>>2)],
        };
      }
  
      function makeRenderPassDescriptor(descriptor) {
        assert(descriptor);
        var nextInChainPtr = HEAPU32[((descriptor)>>2)];
  
        var maxDrawCount = undefined;
        if (nextInChainPtr !== 0) {
          var sType = HEAPU32[(((nextInChainPtr)+(4))>>2)];
          assert(sType === 15);
          assert(0 === HEAPU32[((nextInChainPtr)>>2)]);
          var renderPassDescriptorMaxDrawCount = nextInChainPtr;
          assert(renderPassDescriptorMaxDrawCount);assert(HEAPU32[((renderPassDescriptorMaxDrawCount)>>2)] === 0);
          maxDrawCount = HEAPU32[((((renderPassDescriptorMaxDrawCount + 4))+(8))>>2)] * 0x100000000 + HEAPU32[(((renderPassDescriptorMaxDrawCount)+(8))>>2)];
        }
  
        var desc = {
          "label": undefined,
          "colorAttachments": makeColorAttachments(
            HEAPU32[(((descriptor)+(8))>>2)],
            HEAPU32[(((descriptor)+(12))>>2)]),
          "depthStencilAttachment": makeDepthStencilAttachment(
            HEAPU32[(((descriptor)+(16))>>2)]),
          "occlusionQuerySet": WebGPU.mgrQuerySet.get(
            HEAPU32[(((descriptor)+(20))>>2)]),
          "timestampWrites": makeRenderPassTimestampWrites(
            HEAPU32[(((descriptor)+(24))>>2)]),
            "maxDrawCount": maxDrawCount,
        };
        var labelPtr = HEAPU32[(((descriptor)+(4))>>2)];
        if (labelPtr) desc["label"] = UTF8ToString(labelPtr);
  
        return desc;
      }
  
      var desc = makeRenderPassDescriptor(descriptor);
  
      var commandEncoder = WebGPU.mgrCommandEncoder.get(encoderId);
      return WebGPU.mgrRenderPassEncoder.create(commandEncoder.beginRenderPass(desc));
    };

  var _wgpuCommandEncoderFinish = (encoderId, descriptor) => {
      // TODO: Use the descriptor.
      var commandEncoder = WebGPU.mgrCommandEncoder.get(encoderId);
      return WebGPU.mgrCommandBuffer.create(commandEncoder.finish());
    };

  var _wgpuCommandEncoderRelease = (id) => WebGPU.mgrCommandEncoder.release(id);

  var readI53FromI64 = (ptr) => {
      return HEAPU32[((ptr)>>2)] + HEAP32[(((ptr)+(4))>>2)] * 4294967296;
    };
  
  
  var _wgpuDeviceCreateBindGroup = (deviceId, descriptor) => {
      assert(descriptor);assert(HEAPU32[((descriptor)>>2)] === 0);
  
      function makeEntry(entryPtr) {
        assert(entryPtr);
  
        var bufferId = HEAPU32[(((entryPtr)+(8))>>2)];
        var samplerId = HEAPU32[(((entryPtr)+(32))>>2)];
        var textureViewId = HEAPU32[(((entryPtr)+(36))>>2)];
        assert((bufferId !== 0) + (samplerId !== 0) + (textureViewId !== 0) === 1);
  
        var binding = HEAPU32[(((entryPtr)+(4))>>2)];
  
        if (bufferId) {
          var size = readI53FromI64((entryPtr)+(24));
          if (size == -1) size = undefined;
  
          return {
            "binding": binding,
            "resource": {
              "buffer": WebGPU.mgrBuffer.get(bufferId),
              "offset": HEAPU32[((((entryPtr + 4))+(16))>>2)] * 0x100000000 + HEAPU32[(((entryPtr)+(16))>>2)],
              "size": size
            },
          };
        } else if (samplerId) {
          return {
            "binding": binding,
            "resource": WebGPU.mgrSampler.get(samplerId),
          };
        } else {
          return {
            "binding": binding,
            "resource": WebGPU.mgrTextureView.get(textureViewId),
          };
        }
      }
  
      function makeEntries(count, entriesPtrs) {
        var entries = [];
        for (var i = 0; i < count; ++i) {
          entries.push(makeEntry(entriesPtrs +
              40 * i));
        }
        return entries;
      }
  
      var desc = {
        "label": undefined,
        "layout": WebGPU.mgrBindGroupLayout.get(
          HEAPU32[(((descriptor)+(8))>>2)]),
        "entries": makeEntries(
          HEAPU32[(((descriptor)+(12))>>2)],
          HEAPU32[(((descriptor)+(16))>>2)]
        ),
      };
      var labelPtr = HEAPU32[(((descriptor)+(4))>>2)];
      if (labelPtr) desc["label"] = UTF8ToString(labelPtr);
  
      var device = WebGPU.mgrDevice.get(deviceId);
      return WebGPU.mgrBindGroup.create(device.createBindGroup(desc));
    };

  
  var _wgpuDeviceCreateBindGroupLayout = (deviceId, descriptor) => {
      assert(descriptor);assert(HEAPU32[((descriptor)>>2)] === 0);
  
      function makeBufferEntry(entryPtr) {
        assert(entryPtr);
  
        var typeInt =
          HEAPU32[(((entryPtr)+(4))>>2)];
        if (!typeInt) return undefined;
  
        return {
          "type": WebGPU.BufferBindingType[typeInt],
          "hasDynamicOffset":
            !!(HEAPU32[(((entryPtr)+(8))>>2)]),
          "minBindingSize":
            HEAPU32[((((entryPtr + 4))+(16))>>2)] * 0x100000000 + HEAPU32[(((entryPtr)+(16))>>2)],
        };
      }
  
      function makeSamplerEntry(entryPtr) {
        assert(entryPtr);
  
        var typeInt =
          HEAPU32[(((entryPtr)+(4))>>2)];
        if (!typeInt) return undefined;
  
        return {
          "type": WebGPU.SamplerBindingType[typeInt],
        };
      }
  
      function makeTextureEntry(entryPtr) {
        assert(entryPtr);
  
        var sampleTypeInt =
          HEAPU32[(((entryPtr)+(4))>>2)];
        if (!sampleTypeInt) return undefined;
  
        return {
          "sampleType": WebGPU.TextureSampleType[sampleTypeInt],
          "viewDimension": WebGPU.TextureViewDimension[
            HEAPU32[(((entryPtr)+(8))>>2)]],
          "multisampled":
            !!(HEAPU32[(((entryPtr)+(12))>>2)]),
        };
      }
  
      function makeStorageTextureEntry(entryPtr) {
        assert(entryPtr);
  
        var accessInt =
          HEAPU32[(((entryPtr)+(4))>>2)]
        if (!accessInt) return undefined;
  
        return {
          "access": WebGPU.StorageTextureAccess[accessInt],
          "format": WebGPU.TextureFormat[
            HEAPU32[(((entryPtr)+(8))>>2)]],
          "viewDimension": WebGPU.TextureViewDimension[
            HEAPU32[(((entryPtr)+(12))>>2)]],
        };
      }
  
      function makeEntry(entryPtr) {
        assert(entryPtr);
  
        return {
          "binding":
            HEAPU32[(((entryPtr)+(4))>>2)],
          "visibility":
            HEAPU32[(((entryPtr)+(8))>>2)],
          "buffer": makeBufferEntry(entryPtr + 16),
          "sampler": makeSamplerEntry(entryPtr + 40),
          "texture": makeTextureEntry(entryPtr + 48),
          "storageTexture": makeStorageTextureEntry(entryPtr + 64),
        };
      }
  
      function makeEntries(count, entriesPtrs) {
        var entries = [];
        for (var i = 0; i < count; ++i) {
          entries.push(makeEntry(entriesPtrs +
              80 * i));
        }
        return entries;
      }
  
      var desc = {
        "entries": makeEntries(
          HEAPU32[(((descriptor)+(8))>>2)],
          HEAPU32[(((descriptor)+(12))>>2)]
        ),
      };
      var labelPtr = HEAPU32[(((descriptor)+(4))>>2)];
      if (labelPtr) desc["label"] = UTF8ToString(labelPtr);
  
      var device = WebGPU.mgrDevice.get(deviceId);
      return WebGPU.mgrBindGroupLayout.create(device.createBindGroupLayout(desc));
    };

  
  var _wgpuDeviceCreateBuffer = (deviceId, descriptor) => {
      assert(descriptor);assert(HEAPU32[((descriptor)>>2)] === 0);
  
      var mappedAtCreation = !!(HEAPU32[(((descriptor)+(24))>>2)]);
  
      var desc = {
        "label": undefined,
        "usage": HEAPU32[(((descriptor)+(8))>>2)],
        "size": HEAPU32[((((descriptor + 4))+(16))>>2)] * 0x100000000 + HEAPU32[(((descriptor)+(16))>>2)],
        "mappedAtCreation": mappedAtCreation,
      };
      var labelPtr = HEAPU32[(((descriptor)+(4))>>2)];
      if (labelPtr) desc["label"] = UTF8ToString(labelPtr);
  
      var device = WebGPU.mgrDevice.get(deviceId);
      var bufferWrapper = {};
      var id = WebGPU.mgrBuffer.create(device.createBuffer(desc), bufferWrapper);
      if (mappedAtCreation) {
        bufferWrapper.mapMode = 2;
        bufferWrapper.onUnmap = [];
      }
      return id;
    };

  
  var _wgpuDeviceCreateCommandEncoder = (deviceId, descriptor) => {
      var desc;
      if (descriptor) {
        assert(descriptor);assert(HEAPU32[((descriptor)>>2)] === 0);
        desc = {
          "label": undefined,
        };
        var labelPtr = HEAPU32[(((descriptor)+(4))>>2)];
        if (labelPtr) desc["label"] = UTF8ToString(labelPtr);
      }
      var device = WebGPU.mgrDevice.get(deviceId);
      return WebGPU.mgrCommandEncoder.create(device.createCommandEncoder(desc));
    };

  
  var _wgpuDeviceCreatePipelineLayout = (deviceId, descriptor) => {
      assert(descriptor);assert(HEAPU32[((descriptor)>>2)] === 0);
      var bglCount = HEAPU32[(((descriptor)+(8))>>2)];
      var bglPtr = HEAPU32[(((descriptor)+(12))>>2)];
      var bgls = [];
      for (var i = 0; i < bglCount; ++i) {
        bgls.push(WebGPU.mgrBindGroupLayout.get(
          HEAPU32[(((bglPtr)+(4 * i))>>2)]));
      }
      var desc = {
        "label": undefined,
        "bindGroupLayouts": bgls,
      };
      var labelPtr = HEAPU32[(((descriptor)+(4))>>2)];
      if (labelPtr) desc["label"] = UTF8ToString(labelPtr);
  
      var device = WebGPU.mgrDevice.get(deviceId);
      return WebGPU.mgrPipelineLayout.create(device.createPipelineLayout(desc));
    };

  
  var generateRenderPipelineDesc = (descriptor) => {
      assert(descriptor);assert(HEAPU32[((descriptor)>>2)] === 0);
      function makePrimitiveState(rsPtr) {
        if (!rsPtr) return undefined;
        assert(rsPtr);
  
        // TODO: This small hack assumes that there's only one type that can be in the chain of
        // WGPUPrimitiveState. The correct thing would be to traverse the chain, but unclippedDepth
        // is going to move into the core object soon, so we'll just do this for now. See:
        // https://github.com/webgpu-native/webgpu-headers/issues/212#issuecomment-1682801259
        var nextInChainPtr = HEAPU32[((rsPtr)>>2)];
        var sType = nextInChainPtr ? HEAPU32[(((nextInChainPtr)+(4))>>2)] : 0;
        
        return {
          "topology": WebGPU.PrimitiveTopology[
            HEAPU32[(((rsPtr)+(4))>>2)]],
          "stripIndexFormat": WebGPU.IndexFormat[
            HEAPU32[(((rsPtr)+(8))>>2)]],
          "frontFace": WebGPU.FrontFace[
            HEAPU32[(((rsPtr)+(12))>>2)]],
          "cullMode": WebGPU.CullMode[
            HEAPU32[(((rsPtr)+(16))>>2)]],
          "unclippedDepth": sType === 7 && !!(HEAPU32[(((nextInChainPtr)+(8))>>2)]),
        };
      }
  
      function makeBlendComponent(bdPtr) {
        if (!bdPtr) return undefined;
        return {
          "operation": WebGPU.BlendOperation[
            HEAPU32[((bdPtr)>>2)]],
          "srcFactor": WebGPU.BlendFactor[
            HEAPU32[(((bdPtr)+(4))>>2)]],
          "dstFactor": WebGPU.BlendFactor[
            HEAPU32[(((bdPtr)+(8))>>2)]],
        };
      }
  
      function makeBlendState(bsPtr) {
        if (!bsPtr) return undefined;
        return {
          "alpha": makeBlendComponent(bsPtr + 12),
          "color": makeBlendComponent(bsPtr + 0),
        };
      }
  
      function makeColorState(csPtr) {
        assert(csPtr);assert(HEAPU32[((csPtr)>>2)] === 0);
        var formatInt = HEAPU32[(((csPtr)+(4))>>2)];
        return formatInt === 0 ? undefined : {
          "format": WebGPU.TextureFormat[formatInt],
          "blend": makeBlendState(HEAPU32[(((csPtr)+(8))>>2)]),
          "writeMask": HEAPU32[(((csPtr)+(12))>>2)],
        };
      }
  
      function makeColorStates(count, csArrayPtr) {
        var states = [];
        for (var i = 0; i < count; ++i) {
          states.push(makeColorState(csArrayPtr + 16 * i));
        }
        return states;
      }
  
      function makeStencilStateFace(ssfPtr) {
        assert(ssfPtr);
        return {
          "compare": WebGPU.CompareFunction[
            HEAPU32[((ssfPtr)>>2)]],
          "failOp": WebGPU.StencilOperation[
            HEAPU32[(((ssfPtr)+(4))>>2)]],
          "depthFailOp": WebGPU.StencilOperation[
            HEAPU32[(((ssfPtr)+(8))>>2)]],
          "passOp": WebGPU.StencilOperation[
            HEAPU32[(((ssfPtr)+(12))>>2)]],
        };
      }
  
      function makeDepthStencilState(dssPtr) {
        if (!dssPtr) return undefined;
  
        assert(dssPtr);
        return {
          "format": WebGPU.TextureFormat[
            HEAPU32[(((dssPtr)+(4))>>2)]],
          "depthWriteEnabled": !!(HEAPU32[(((dssPtr)+(8))>>2)]),
          "depthCompare": WebGPU.CompareFunction[
            HEAPU32[(((dssPtr)+(12))>>2)]],
          "stencilFront": makeStencilStateFace(dssPtr + 16),
          "stencilBack": makeStencilStateFace(dssPtr + 32),
          "stencilReadMask": HEAPU32[(((dssPtr)+(48))>>2)],
          "stencilWriteMask": HEAPU32[(((dssPtr)+(52))>>2)],
          "depthBias": HEAP32[(((dssPtr)+(56))>>2)],
          "depthBiasSlopeScale": HEAPF32[(((dssPtr)+(60))>>2)],
          "depthBiasClamp": HEAPF32[(((dssPtr)+(64))>>2)],
        };
      }
  
      function makeVertexAttribute(vaPtr) {
        assert(vaPtr);
        return {
          "format": WebGPU.VertexFormat[
            HEAPU32[((vaPtr)>>2)]],
          "offset": HEAPU32[((((vaPtr + 4))+(8))>>2)] * 0x100000000 + HEAPU32[(((vaPtr)+(8))>>2)],
          "shaderLocation": HEAPU32[(((vaPtr)+(16))>>2)],
        };
      }
  
      function makeVertexAttributes(count, vaArrayPtr) {
        var vas = [];
        for (var i = 0; i < count; ++i) {
          vas.push(makeVertexAttribute(vaArrayPtr + i * 24));
        }
        return vas;
      }
  
      function makeVertexBuffer(vbPtr) {
        if (!vbPtr) return undefined;
        var stepModeInt = HEAPU32[(((vbPtr)+(8))>>2)];
        return stepModeInt === 1 ? null : {
          "arrayStride": HEAPU32[(((vbPtr + 4))>>2)] * 0x100000000 + HEAPU32[((vbPtr)>>2)],
          "stepMode": WebGPU.VertexStepMode[stepModeInt],
          "attributes": makeVertexAttributes(
            HEAPU32[(((vbPtr)+(12))>>2)],
            HEAPU32[(((vbPtr)+(16))>>2)]),
        };
      }
  
      function makeVertexBuffers(count, vbArrayPtr) {
        if (!count) return undefined;
  
        var vbs = [];
        for (var i = 0; i < count; ++i) {
          vbs.push(makeVertexBuffer(vbArrayPtr + i * 24));
        }
        return vbs;
      }
  
      function makeVertexState(viPtr) {
        if (!viPtr) return undefined;
        assert(viPtr);assert(HEAPU32[((viPtr)>>2)] === 0);
        var desc = {
          "module": WebGPU.mgrShaderModule.get(
            HEAPU32[(((viPtr)+(4))>>2)]),
          "constants": WebGPU.makePipelineConstants(
            HEAPU32[(((viPtr)+(12))>>2)],
            HEAPU32[(((viPtr)+(16))>>2)]),
          "buffers": makeVertexBuffers(
            HEAPU32[(((viPtr)+(20))>>2)],
            HEAPU32[(((viPtr)+(24))>>2)]),
          };
        var entryPointPtr = HEAPU32[(((viPtr)+(8))>>2)];
        if (entryPointPtr) desc["entryPoint"] = UTF8ToString(entryPointPtr);
        return desc;
      }
  
      function makeMultisampleState(msPtr) {
        if (!msPtr) return undefined;
        assert(msPtr);assert(HEAPU32[((msPtr)>>2)] === 0);
        return {
          "count": HEAPU32[(((msPtr)+(4))>>2)],
          "mask": HEAPU32[(((msPtr)+(8))>>2)],
          "alphaToCoverageEnabled": !!(HEAPU32[(((msPtr)+(12))>>2)]),
        };
      }
  
      function makeFragmentState(fsPtr) {
        if (!fsPtr) return undefined;
        assert(fsPtr);assert(HEAPU32[((fsPtr)>>2)] === 0);
        var desc = {
          "module": WebGPU.mgrShaderModule.get(
            HEAPU32[(((fsPtr)+(4))>>2)]),
          "constants": WebGPU.makePipelineConstants(
            HEAPU32[(((fsPtr)+(12))>>2)],
            HEAPU32[(((fsPtr)+(16))>>2)]),
          "targets": makeColorStates(
            HEAPU32[(((fsPtr)+(20))>>2)],
            HEAPU32[(((fsPtr)+(24))>>2)]),
          };
        var entryPointPtr = HEAPU32[(((fsPtr)+(8))>>2)];
        if (entryPointPtr) desc["entryPoint"] = UTF8ToString(entryPointPtr);
        return desc;
      }
  
      var desc = {
        "label": undefined,
        "layout": WebGPU.makePipelineLayout(
          HEAPU32[(((descriptor)+(8))>>2)]),
        "vertex": makeVertexState(
          descriptor + 12),
        "primitive": makePrimitiveState(
          descriptor + 40),
        "depthStencil": makeDepthStencilState(
          HEAPU32[(((descriptor)+(60))>>2)]),
        "multisample": makeMultisampleState(
          descriptor + 64),
        "fragment": makeFragmentState(
          HEAPU32[(((descriptor)+(80))>>2)]),
      };
      var labelPtr = HEAPU32[(((descriptor)+(4))>>2)];
      if (labelPtr) desc["label"] = UTF8ToString(labelPtr);
      return desc;
    };
  
  var _wgpuDeviceCreateRenderPipeline = (deviceId, descriptor) => {
      var desc = generateRenderPipelineDesc(descriptor);
      var device = WebGPU.mgrDevice.get(deviceId);
      return WebGPU.mgrRenderPipeline.create(device.createRenderPipeline(desc));
    };

  
  var _wgpuDeviceCreateShaderModule = (deviceId, descriptor) => {
      assert(descriptor);
      var nextInChainPtr = HEAPU32[((descriptor)>>2)];
      assert(nextInChainPtr !== 0);
      var sType = HEAPU32[(((nextInChainPtr)+(4))>>2)];
  
      var desc = {
        "label": undefined,
        "code": "",
      };
      var labelPtr = HEAPU32[(((descriptor)+(4))>>2)];
      if (labelPtr) desc["label"] = UTF8ToString(labelPtr);
  
      switch (sType) {
        case 5: {
          var count = HEAPU32[(((nextInChainPtr)+(8))>>2)];
          var start = HEAPU32[(((nextInChainPtr)+(12))>>2)];
          var offset = ((start)>>2);
          desc["code"] = HEAPU32.subarray(offset, offset + count);
          break;
        }
        case 6: {
          var sourcePtr = HEAPU32[(((nextInChainPtr)+(8))>>2)];
          if (sourcePtr) {
            desc["code"] = UTF8ToString(sourcePtr);
          }
          break;
        }
        default: abort('unrecognized ShaderModule sType');
      }
  
      var device = WebGPU.mgrDevice.get(deviceId);
      return WebGPU.mgrShaderModule.create(device.createShaderModule(desc));
    };

  var _wgpuDeviceCreateSwapChain = (deviceId, surfaceId, descriptor) => {
      assert(descriptor);assert(HEAPU32[((descriptor)>>2)] === 0);
      var device = WebGPU.mgrDevice.get(deviceId);
      var context = WebGPU.mgrSurface.get(surfaceId);
  
      assert(1 ===
        HEAPU32[(((descriptor)+(24))>>2)]);
  
      var canvasSize = [
        HEAPU32[(((descriptor)+(16))>>2)],
        HEAPU32[(((descriptor)+(20))>>2)]
      ];
  
      if (canvasSize[0] !== 0) {
        context["canvas"]["width"] = canvasSize[0];
      }
  
      if (canvasSize[1] !== 0) {
        context["canvas"]["height"] = canvasSize[1];
      }
  
      var configuration = {
        "device": device,
        "format": WebGPU.TextureFormat[
          HEAPU32[(((descriptor)+(12))>>2)]],
        "usage": HEAPU32[(((descriptor)+(8))>>2)],
        "alphaMode": "opaque",
      };
      context.configure(configuration);
  
      return WebGPU.mgrSwapChain.create(context);
    };

  var _wgpuDeviceGetQueue = (deviceId) => {
      var queueId = WebGPU.mgrDevice.objects[deviceId].queueId;
      assert(queueId, 'wgpuDeviceGetQueue: queue was missing or null');
      // Returns a new reference to the existing queue.
      WebGPU.mgrQueue.reference(queueId);
      return queueId;
    };

  var _wgpuDeviceRelease = (id) => WebGPU.mgrDevice.release(id);

  var findCanvasEventTarget = findEventTarget;
  
  
  var _wgpuInstanceCreateSurface = (instanceId, descriptor) => {
      assert(descriptor);
      assert(instanceId === 1, "WGPUInstance must be created by wgpuCreateInstance");
      var nextInChainPtr = HEAPU32[((descriptor)>>2)];
      assert(nextInChainPtr !== 0);
      assert(4 ===
        HEAPU32[(((nextInChainPtr)+(4))>>2)]);
      var descriptorFromCanvasHTMLSelector = nextInChainPtr;
  
      assert(descriptorFromCanvasHTMLSelector);assert(HEAPU32[((descriptorFromCanvasHTMLSelector)>>2)] === 0);
      var selectorPtr = HEAPU32[(((descriptorFromCanvasHTMLSelector)+(8))>>2)];
      assert(selectorPtr);
      var canvas = findCanvasEventTarget(selectorPtr);
      var context = canvas.getContext('webgpu');
      assert(context);
      if (!context) return 0;
  
      var labelPtr = HEAPU32[(((descriptor)+(4))>>2)];
      if (labelPtr) context.surfaceLabelWebGPU = UTF8ToString(labelPtr);
  
      return WebGPU.mgrSurface.create(context);
    };

  var _wgpuPipelineLayoutRelease = (id) => WebGPU.mgrPipelineLayout.release(id);

  var _wgpuQueueRelease = (id) => WebGPU.mgrQueue.release(id);

  var _wgpuQueueSubmit = (queueId, commandCount, commands) => {
      assert(commands % 4 === 0);
      var queue = WebGPU.mgrQueue.get(queueId);
      var cmds = Array.from(HEAP32.subarray((((commands)>>2)), ((commands + commandCount * 4)>>2)),
        (id) => WebGPU.mgrCommandBuffer.get(id));
      queue.submit(cmds);
    };

  
  function _wgpuQueueWriteBuffer(queueId, bufferId, bufferOffset, data, size) {
    bufferOffset = bigintToI53Checked(bufferOffset);
  
    
      var queue = WebGPU.mgrQueue.get(queueId);
      var buffer = WebGPU.mgrBuffer.get(bufferId);
      // There is a size limitation for ArrayBufferView. Work around by passing in a subarray
      // instead of the whole heap. crbug.com/1201109
      var subarray = HEAPU8.subarray(data, data + size);
      queue.writeBuffer(buffer, bufferOffset, subarray, 0, size);
    ;
  }

  var _wgpuRenderPassEncoderDrawIndexed = (passId, indexCount, instanceCount, firstIndex, baseVertex, firstInstance) => {
      var pass = WebGPU.mgrRenderPassEncoder.get(passId);
      pass.drawIndexed(indexCount, instanceCount, firstIndex, baseVertex, firstInstance);
    };

  var _wgpuRenderPassEncoderEnd = (encoderId) => {
      var encoder = WebGPU.mgrRenderPassEncoder.get(encoderId);
      encoder.end();
    };

  var _wgpuRenderPassEncoderRelease = (id) => WebGPU.mgrRenderPassEncoder.release(id);

  var _wgpuRenderPassEncoderSetBindGroup = (passId, groupIndex, groupId, dynamicOffsetCount, dynamicOffsetsPtr) => {
      var pass = WebGPU.mgrRenderPassEncoder.get(passId);
      var group = WebGPU.mgrBindGroup.get(groupId);
      if (dynamicOffsetCount == 0) {
        pass.setBindGroup(groupIndex, group);
      } else {
        var offsets = [];
        for (var i = 0; i < dynamicOffsetCount; i++, dynamicOffsetsPtr += 4) {
          offsets.push(HEAPU32[((dynamicOffsetsPtr)>>2)]);
        }
        pass.setBindGroup(groupIndex, group, offsets);
      }
    };

  
  function _wgpuRenderPassEncoderSetIndexBuffer(passId, bufferId, format, offset, size) {
    offset = bigintToI53Checked(offset);
    size = bigintToI53Checked(size);
  
    
      var pass = WebGPU.mgrRenderPassEncoder.get(passId);
      var buffer = WebGPU.mgrBuffer.get(bufferId);
      if (size == -1) size = undefined;
      pass.setIndexBuffer(buffer, WebGPU.IndexFormat[format], offset, size);
    ;
  }

  var _wgpuRenderPassEncoderSetPipeline = (passId, pipelineId) => {
      var pass = WebGPU.mgrRenderPassEncoder.get(passId);
      var pipeline = WebGPU.mgrRenderPipeline.get(pipelineId);
      pass.setPipeline(pipeline);
    };

  
  function _wgpuRenderPassEncoderSetVertexBuffer(passId, slot, bufferId, offset, size) {
    offset = bigintToI53Checked(offset);
    size = bigintToI53Checked(size);
  
    
      var pass = WebGPU.mgrRenderPassEncoder.get(passId);
      var buffer = WebGPU.mgrBuffer.get(bufferId);
      if (size == -1) size = undefined;
      pass.setVertexBuffer(slot, buffer, offset, size);
    ;
  }

  var _wgpuRenderPipelineGetBindGroupLayout = (pipelineId, groupIndex) => {
      var pipeline = WebGPU.mgrRenderPipeline.get(pipelineId);
      return WebGPU.mgrBindGroupLayout.create(pipeline.getBindGroupLayout(groupIndex));
    };

  var _wgpuRenderPipelineRelease = (id) => WebGPU.mgrRenderPipeline.release(id);

  var _wgpuShaderModuleRelease = (id) => WebGPU.mgrShaderModule.release(id);

  var _wgpuSwapChainGetCurrentTextureView = (swapChainId) => {
      var context = WebGPU.mgrSwapChain.get(swapChainId);
      return WebGPU.mgrTextureView.create(context.getCurrentTexture().createView());
    };

  var _wgpuSwapChainRelease = (id) => WebGPU.mgrSwapChain.release(id);

  var _wgpuTextureViewRelease = (id) => WebGPU.mgrTextureView.release(id);





  var getCFunc = (ident) => {
      var func = Module['_' + ident]; // closure exported function
      assert(func, 'Cannot call unknown function ' + ident + ', make sure it is exported');
      return func;
    };
  
  var writeArrayToMemory = (array, buffer) => {
      assert(array.length >= 0, 'writeArrayToMemory array must have a length (should be an array or typed array)')
      HEAP8.set(array, buffer);
    };
  
  
  
  
  
  
    /**
     * @param {string|null=} returnType
     * @param {Array=} argTypes
     * @param {Arguments|Array=} args
     * @param {Object=} opts
     */
  var ccall = (ident, returnType, argTypes, args, opts) => {
      // For fast lookup of conversion functions
      var toC = {
        'string': (str) => {
          var ret = 0;
          if (str !== null && str !== undefined && str !== 0) { // null string
            ret = stringToUTF8OnStack(str);
          }
          return ret;
        },
        'array': (arr) => {
          var ret = stackAlloc(arr.length);
          writeArrayToMemory(arr, ret);
          return ret;
        }
      };
  
      function convertReturnValue(ret) {
        if (returnType === 'string') {
          return UTF8ToString(ret);
        }
        if (returnType === 'boolean') return Boolean(ret);
        return ret;
      }
  
      var func = getCFunc(ident);
      var cArgs = [];
      var stack = 0;
      assert(returnType !== 'array', 'Return type should not be "array".');
      if (args) {
        for (var i = 0; i < args.length; i++) {
          var converter = toC[argTypes[i]];
          if (converter) {
            if (stack === 0) stack = stackSave();
            cArgs[i] = converter(args[i]);
          } else {
            cArgs[i] = args[i];
          }
        }
      }
      var ret = func(...cArgs);
      function onDone(ret) {
        if (stack !== 0) stackRestore(stack);
        return convertReturnValue(ret);
      }
  
      ret = onDone(ret);
      return ret;
    };

  var FS_createPath = FS.createPath;



  var FS_unlink = (path) => FS.unlink(path);

  var FS_createLazyFile = FS.createLazyFile;

  var FS_createDevice = FS.createDevice;

  FS.createPreloadedFile = FS_createPreloadedFile;
  FS.staticInit();
  // Set module methods based on EXPORTED_RUNTIME_METHODS
  Module["FS_createPath"] = FS.createPath;
  Module["FS_createDataFile"] = FS.createDataFile;
  Module["FS_createPreloadedFile"] = FS.createPreloadedFile;
  Module["FS_unlink"] = FS.unlink;
  Module["FS_createLazyFile"] = FS.createLazyFile;
  Module["FS_createDevice"] = FS.createDevice;
  ;

      Module["requestAnimationFrame"] = MainLoop.requestAnimationFrame;
      Module["pauseMainLoop"] = MainLoop.pause;
      Module["resumeMainLoop"] = MainLoop.resume;
      MainLoop.init();;
WebGPU.initManagers();;
function checkIncomingModuleAPI() {
  ignoredModuleProp('fetchSettings');
}
var wasmImports = {
  /** @export */
  __assert_fail: ___assert_fail,
  /** @export */
  __syscall_fcntl64: ___syscall_fcntl64,
  /** @export */
  __syscall_ioctl: ___syscall_ioctl,
  /** @export */
  __syscall_openat: ___syscall_openat,
  /** @export */
  _abort_js: __abort_js,
  /** @export */
  clock_time_get: _clock_time_get,
  /** @export */
  emscripten_get_element_css_size: _emscripten_get_element_css_size,
  /** @export */
  emscripten_request_pointerlock: _emscripten_request_pointerlock,
  /** @export */
  emscripten_resize_heap: _emscripten_resize_heap,
  /** @export */
  emscripten_set_element_css_size: _emscripten_set_element_css_size,
  /** @export */
  emscripten_set_keydown_callback_on_thread: _emscripten_set_keydown_callback_on_thread,
  /** @export */
  emscripten_set_keyup_callback_on_thread: _emscripten_set_keyup_callback_on_thread,
  /** @export */
  emscripten_set_main_loop: _emscripten_set_main_loop,
  /** @export */
  emscripten_set_mousemove_callback_on_thread: _emscripten_set_mousemove_callback_on_thread,
  /** @export */
  emscripten_set_resize_callback_on_thread: _emscripten_set_resize_callback_on_thread,
  /** @export */
  emscripten_set_wheel_callback_on_thread: _emscripten_set_wheel_callback_on_thread,
  /** @export */
  emscripten_webgpu_get_device: _emscripten_webgpu_get_device,
  /** @export */
  exit: _exit,
  /** @export */
  fd_close: _fd_close,
  /** @export */
  fd_read: _fd_read,
  /** @export */
  fd_seek: _fd_seek,
  /** @export */
  fd_write: _fd_write,
  /** @export */
  wgpuBindGroupLayoutRelease: _wgpuBindGroupLayoutRelease,
  /** @export */
  wgpuCommandBufferRelease: _wgpuCommandBufferRelease,
  /** @export */
  wgpuCommandEncoderBeginRenderPass: _wgpuCommandEncoderBeginRenderPass,
  /** @export */
  wgpuCommandEncoderFinish: _wgpuCommandEncoderFinish,
  /** @export */
  wgpuCommandEncoderRelease: _wgpuCommandEncoderRelease,
  /** @export */
  wgpuDeviceCreateBindGroup: _wgpuDeviceCreateBindGroup,
  /** @export */
  wgpuDeviceCreateBindGroupLayout: _wgpuDeviceCreateBindGroupLayout,
  /** @export */
  wgpuDeviceCreateBuffer: _wgpuDeviceCreateBuffer,
  /** @export */
  wgpuDeviceCreateCommandEncoder: _wgpuDeviceCreateCommandEncoder,
  /** @export */
  wgpuDeviceCreatePipelineLayout: _wgpuDeviceCreatePipelineLayout,
  /** @export */
  wgpuDeviceCreateRenderPipeline: _wgpuDeviceCreateRenderPipeline,
  /** @export */
  wgpuDeviceCreateShaderModule: _wgpuDeviceCreateShaderModule,
  /** @export */
  wgpuDeviceCreateSwapChain: _wgpuDeviceCreateSwapChain,
  /** @export */
  wgpuDeviceGetQueue: _wgpuDeviceGetQueue,
  /** @export */
  wgpuDeviceRelease: _wgpuDeviceRelease,
  /** @export */
  wgpuInstanceCreateSurface: _wgpuInstanceCreateSurface,
  /** @export */
  wgpuPipelineLayoutRelease: _wgpuPipelineLayoutRelease,
  /** @export */
  wgpuQueueRelease: _wgpuQueueRelease,
  /** @export */
  wgpuQueueSubmit: _wgpuQueueSubmit,
  /** @export */
  wgpuQueueWriteBuffer: _wgpuQueueWriteBuffer,
  /** @export */
  wgpuRenderPassEncoderDrawIndexed: _wgpuRenderPassEncoderDrawIndexed,
  /** @export */
  wgpuRenderPassEncoderEnd: _wgpuRenderPassEncoderEnd,
  /** @export */
  wgpuRenderPassEncoderRelease: _wgpuRenderPassEncoderRelease,
  /** @export */
  wgpuRenderPassEncoderSetBindGroup: _wgpuRenderPassEncoderSetBindGroup,
  /** @export */
  wgpuRenderPassEncoderSetIndexBuffer: _wgpuRenderPassEncoderSetIndexBuffer,
  /** @export */
  wgpuRenderPassEncoderSetPipeline: _wgpuRenderPassEncoderSetPipeline,
  /** @export */
  wgpuRenderPassEncoderSetVertexBuffer: _wgpuRenderPassEncoderSetVertexBuffer,
  /** @export */
  wgpuRenderPipelineGetBindGroupLayout: _wgpuRenderPipelineGetBindGroupLayout,
  /** @export */
  wgpuRenderPipelineRelease: _wgpuRenderPipelineRelease,
  /** @export */
  wgpuShaderModuleRelease: _wgpuShaderModuleRelease,
  /** @export */
  wgpuSwapChainGetCurrentTextureView: _wgpuSwapChainGetCurrentTextureView,
  /** @export */
  wgpuSwapChainRelease: _wgpuSwapChainRelease,
  /** @export */
  wgpuTextureViewRelease: _wgpuTextureViewRelease
};
var wasmExports;
createWasm();
var ___wasm_call_ctors = createExportWrapper('__wasm_call_ctors', 0);
var _malloc = createExportWrapper('malloc', 1);
var _main = Module['_main'] = createExportWrapper('__main_argc_argv', 2);
var _fflush = createExportWrapper('fflush', 1);
var _strerror = createExportWrapper('strerror', 1);
var _emscripten_stack_init = () => (_emscripten_stack_init = wasmExports['emscripten_stack_init'])();
var _emscripten_stack_get_free = () => (_emscripten_stack_get_free = wasmExports['emscripten_stack_get_free'])();
var _emscripten_stack_get_base = () => (_emscripten_stack_get_base = wasmExports['emscripten_stack_get_base'])();
var _emscripten_stack_get_end = () => (_emscripten_stack_get_end = wasmExports['emscripten_stack_get_end'])();
var __emscripten_stack_restore = (a0) => (__emscripten_stack_restore = wasmExports['_emscripten_stack_restore'])(a0);
var __emscripten_stack_alloc = (a0) => (__emscripten_stack_alloc = wasmExports['_emscripten_stack_alloc'])(a0);
var _emscripten_stack_get_current = () => (_emscripten_stack_get_current = wasmExports['emscripten_stack_get_current'])();


// include: postamble.js
// === Auto-generated postamble setup entry stuff ===

Module['addRunDependency'] = addRunDependency;
Module['removeRunDependency'] = removeRunDependency;
Module['ccall'] = ccall;
Module['FS_createPreloadedFile'] = FS_createPreloadedFile;
Module['FS_unlink'] = FS_unlink;
Module['FS_createPath'] = FS_createPath;
Module['FS_createDevice'] = FS_createDevice;
Module['FS_createDataFile'] = FS_createDataFile;
Module['FS_createLazyFile'] = FS_createLazyFile;
var missingLibrarySymbols = [
  'writeI53ToI64',
  'writeI53ToI64Clamped',
  'writeI53ToI64Signaling',
  'writeI53ToU64Clamped',
  'writeI53ToU64Signaling',
  'readI53FromU64',
  'convertI32PairToI53',
  'convertI32PairToI53Checked',
  'convertU32PairToI53',
  'getTempRet0',
  'setTempRet0',
  'growMemory',
  'inetPton4',
  'inetNtop4',
  'inetPton6',
  'inetNtop6',
  'readSockaddr',
  'writeSockaddr',
  'emscriptenLog',
  'readEmAsmArgs',
  'jstoi_q',
  'getExecutableName',
  'listenOnce',
  'autoResumeAudioContext',
  'getDynCaller',
  'dynCall',
  'runtimeKeepalivePush',
  'runtimeKeepalivePop',
  'asmjsMangle',
  'HandleAllocator',
  'getNativeTypeSize',
  'STACK_SIZE',
  'STACK_ALIGN',
  'POINTER_SIZE',
  'ASSERTIONS',
  'cwrap',
  'uleb128Encode',
  'sigToWasmTypes',
  'generateFuncType',
  'convertJsFunctionToWasm',
  'getEmptyTableSlot',
  'updateTableMap',
  'getFunctionAddress',
  'addFunction',
  'removeFunction',
  'reallyNegative',
  'unSign',
  'strLen',
  'reSign',
  'formatString',
  'intArrayToString',
  'AsciiToString',
  'stringToAscii',
  'UTF16ToString',
  'stringToUTF16',
  'lengthBytesUTF16',
  'UTF32ToString',
  'stringToUTF32',
  'lengthBytesUTF32',
  'stringToNewUTF8',
  'registerFocusEventCallback',
  'fillDeviceOrientationEventData',
  'registerDeviceOrientationEventCallback',
  'fillDeviceMotionEventData',
  'registerDeviceMotionEventCallback',
  'screenOrientation',
  'fillOrientationChangeEventData',
  'registerOrientationChangeEventCallback',
  'fillFullscreenChangeEventData',
  'registerFullscreenChangeEventCallback',
  'JSEvents_requestFullscreen',
  'JSEvents_resizeCanvasForFullscreen',
  'registerRestoreOldStyle',
  'hideEverythingExceptGivenElement',
  'restoreHiddenElements',
  'setLetterbox',
  'softFullscreenResizeWebGLRenderTarget',
  'doRequestFullscreen',
  'fillPointerlockChangeEventData',
  'registerPointerlockChangeEventCallback',
  'registerPointerlockErrorEventCallback',
  'fillVisibilityChangeEventData',
  'registerVisibilityChangeEventCallback',
  'registerTouchEventCallback',
  'fillGamepadEventData',
  'registerGamepadEventCallback',
  'registerBeforeUnloadEventCallback',
  'fillBatteryEventData',
  'battery',
  'registerBatteryEventCallback',
  'setCanvasElementSize',
  'getCanvasElementSize',
  'jsStackTrace',
  'getCallstack',
  'convertPCtoSourceLocation',
  'getEnvStrings',
  'wasiRightsToMuslOFlags',
  'wasiOFlagsToMuslOFlags',
  'safeSetTimeout',
  'setImmediateWrapped',
  'safeRequestAnimationFrame',
  'clearImmediateWrapped',
  'registerPostMainLoop',
  'registerPreMainLoop',
  'getPromise',
  'makePromise',
  'idsToPromises',
  'makePromiseCallback',
  'ExceptionInfo',
  'findMatchingCatch',
  'Browser_asyncPrepareDataCounter',
  'isLeapYear',
  'ydayFromDate',
  'arraySum',
  'addDays',
  'getSocketFromFD',
  'getSocketAddress',
  'FS_mkdirTree',
  '_setNetworkCallback',
  'heapObjectForWebGLType',
  'toTypedArrayIndex',
  'webgl_enable_ANGLE_instanced_arrays',
  'webgl_enable_OES_vertex_array_object',
  'webgl_enable_WEBGL_draw_buffers',
  'webgl_enable_WEBGL_multi_draw',
  'webgl_enable_EXT_polygon_offset_clamp',
  'webgl_enable_EXT_clip_control',
  'webgl_enable_WEBGL_polygon_mode',
  'emscriptenWebGLGet',
  'computeUnpackAlignedImageSize',
  'colorChannelsInGlTextureFormat',
  'emscriptenWebGLGetTexPixelData',
  'emscriptenWebGLGetUniform',
  'webglGetUniformLocation',
  'webglPrepareUniformLocationsBeforeFirstUse',
  'webglGetLeftBracePos',
  'emscriptenWebGLGetVertexAttrib',
  '__glGetActiveAttribOrUniform',
  'writeGLArray',
  'registerWebGlEventCallback',
  'runAndAbortIfError',
  'ALLOC_NORMAL',
  'ALLOC_STACK',
  'allocate',
  'writeStringToMemory',
  'writeAsciiToMemory',
  'setErrNo',
  'demangle',
  'stackTrace',
];
missingLibrarySymbols.forEach(missingLibrarySymbol)

var unexportedSymbols = [
  'run',
  'addOnPreRun',
  'addOnInit',
  'addOnPreMain',
  'addOnExit',
  'addOnPostRun',
  'out',
  'err',
  'callMain',
  'abort',
  'wasmMemory',
  'wasmExports',
  'writeStackCookie',
  'checkStackCookie',
  'readI53FromI64',
  'INT53_MAX',
  'INT53_MIN',
  'bigintToI53Checked',
  'stackSave',
  'stackRestore',
  'stackAlloc',
  'ptrToString',
  'zeroMemory',
  'exitJS',
  'getHeapMax',
  'abortOnCannotGrowMemory',
  'ENV',
  'ERRNO_CODES',
  'strError',
  'DNS',
  'Protocols',
  'Sockets',
  'timers',
  'warnOnce',
  'readEmAsmArgsArray',
  'jstoi_s',
  'handleException',
  'keepRuntimeAlive',
  'callUserCallback',
  'maybeExit',
  'asyncLoad',
  'alignMemory',
  'mmapAlloc',
  'wasmTable',
  'noExitRuntime',
  'getCFunc',
  'freeTableIndexes',
  'functionsInTableMap',
  'setValue',
  'getValue',
  'PATH',
  'PATH_FS',
  'UTF8Decoder',
  'UTF8ArrayToString',
  'UTF8ToString',
  'stringToUTF8Array',
  'stringToUTF8',
  'lengthBytesUTF8',
  'intArrayFromString',
  'UTF16Decoder',
  'stringToUTF8OnStack',
  'writeArrayToMemory',
  'JSEvents',
  'registerKeyEventCallback',
  'specialHTMLTargets',
  'maybeCStringToJsString',
  'findEventTarget',
  'findCanvasEventTarget',
  'getBoundingClientRect',
  'fillMouseEventData',
  'registerMouseEventCallback',
  'registerWheelEventCallback',
  'registerUiEventCallback',
  'currentFullscreenStrategy',
  'restoreOldWindowedStyle',
  'requestPointerLock',
  'UNWIND_CACHE',
  'ExitStatus',
  'checkWasiClock',
  'doReadv',
  'doWritev',
  'initRandomFill',
  'randomFill',
  'emSetImmediate',
  'emClearImmediate_deps',
  'emClearImmediate',
  'promiseMap',
  'uncaughtExceptionCount',
  'exceptionLast',
  'exceptionCaught',
  'Browser',
  'getPreloadedImageData__data',
  'wget',
  'MONTH_DAYS_REGULAR',
  'MONTH_DAYS_LEAP',
  'MONTH_DAYS_REGULAR_CUMULATIVE',
  'MONTH_DAYS_LEAP_CUMULATIVE',
  'SYSCALLS',
  'preloadPlugins',
  'FS_modeStringToFlags',
  'FS_getMode',
  'FS_stdin_getChar_buffer',
  'FS_stdin_getChar',
  'FS_readFile',
  'FS',
  'MEMFS',
  'TTY',
  'PIPEFS',
  'SOCKFS',
  'tempFixedLengthArray',
  'miniTempWebGLFloatBuffers',
  'miniTempWebGLIntBuffers',
  'GL',
  'AL',
  'GLUT',
  'EGL',
  'GLEW',
  'IDBStore',
  'SDL',
  'SDL_gfx',
  'WebGPU',
  'JsValStore',
  'allocateUTF8',
  'allocateUTF8OnStack',
  'print',
  'printErr',
];
unexportedSymbols.forEach(unexportedRuntimeSymbol);



var calledRun;

function callMain(args = []) {
  assert(runDependencies == 0, 'cannot call main when async dependencies remain! (listen on Module["onRuntimeInitialized"])');
  assert(__ATPRERUN__.length == 0, 'cannot call main when preRun functions remain to be called');

  var entryFunction = _main;

  args.unshift(thisProgram);

  var argc = args.length;
  var argv = stackAlloc((argc + 1) * 4);
  var argv_ptr = argv;
  args.forEach((arg) => {
    HEAPU32[((argv_ptr)>>2)] = stringToUTF8OnStack(arg);
    argv_ptr += 4;
  });
  HEAPU32[((argv_ptr)>>2)] = 0;

  try {

    var ret = entryFunction(argc, argv);

    // if we're not running an evented main loop, it's time to exit
    exitJS(ret, /* implicit = */ true);
    return ret;
  } catch (e) {
    return handleException(e);
  }
}

function stackCheckInit() {
  // This is normally called automatically during __wasm_call_ctors but need to
  // get these values before even running any of the ctors so we call it redundantly
  // here.
  _emscripten_stack_init();
  // TODO(sbc): Move writeStackCookie to native to to avoid this.
  writeStackCookie();
}

function run(args = arguments_) {

  if (runDependencies > 0) {
    dependenciesFulfilled = run;
    return;
  }

  stackCheckInit();

  preRun();

  // a preRun added a dependency, run will be called later
  if (runDependencies > 0) {
    dependenciesFulfilled = run;
    return;
  }

  function doRun() {
    // run may have just been called through dependencies being fulfilled just in this very frame,
    // or while the async setStatus time below was happening
    assert(!calledRun);
    calledRun = true;
    Module['calledRun'] = true;

    if (ABORT) return;

    initRuntime();

    preMain();

    Module['onRuntimeInitialized']?.();

    var noInitialRun = Module['noInitialRun'];legacyModuleProp('noInitialRun', 'noInitialRun');
    if (!noInitialRun) callMain(args);

    postRun();
  }

  if (Module['setStatus']) {
    Module['setStatus']('Running...');
    setTimeout(() => {
      setTimeout(() => Module['setStatus'](''), 1);
      doRun();
    }, 1);
  } else
  {
    doRun();
  }
  checkStackCookie();
}

function checkUnflushedContent() {
  // Compiler settings do not allow exiting the runtime, so flushing
  // the streams is not possible. but in ASSERTIONS mode we check
  // if there was something to flush, and if so tell the user they
  // should request that the runtime be exitable.
  // Normally we would not even include flush() at all, but in ASSERTIONS
  // builds we do so just for this check, and here we see if there is any
  // content to flush, that is, we check if there would have been
  // something a non-ASSERTIONS build would have not seen.
  // How we flush the streams depends on whether we are in SYSCALLS_REQUIRE_FILESYSTEM=0
  // mode (which has its own special function for this; otherwise, all
  // the code is inside libc)
  var oldOut = out;
  var oldErr = err;
  var has = false;
  out = err = (x) => {
    has = true;
  }
  try { // it doesn't matter if it fails
    _fflush(0);
    // also flush in the JS FS layer
    ['stdout', 'stderr'].forEach((name) => {
      var info = FS.analyzePath('/dev/' + name);
      if (!info) return;
      var stream = info.object;
      var rdev = stream.rdev;
      var tty = TTY.ttys[rdev];
      if (tty?.output?.length) {
        has = true;
      }
    });
  } catch(e) {}
  out = oldOut;
  err = oldErr;
  if (has) {
    warnOnce('stdio streams had content in them that was not flushed. you should set EXIT_RUNTIME to 1 (see the Emscripten FAQ), or make sure to emit a newline when you printf etc.');
  }
}

if (Module['preInit']) {
  if (typeof Module['preInit'] == 'function') Module['preInit'] = [Module['preInit']];
  while (Module['preInit'].length > 0) {
    Module['preInit'].pop()();
  }
}

run();

// end include: postamble.js

