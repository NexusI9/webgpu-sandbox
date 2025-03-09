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
// include: /var/folders/hx/g36pxlqs1dj0kvmzszq_j3k00000gq/T/tmp2beuh8cn.js

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
    loadPackage({"files": [{"filename": "/resources/assets/gltf/cube.gltf", "start": 0, "end": 9249}, {"filename": "/resources/assets/gltf/ico.gltf", "start": 9249, "end": 21503}, {"filename": "/runtime/assets/shader/shader.default.wgsl", "start": 21503, "end": 22968}, {"filename": "/runtime/assets/shader/shader.grid.wgsl", "start": 22968, "end": 28150}, {"filename": "/runtime/assets/shader/shader.pbr.wgsl", "start": 28150, "end": 29895}], "remote_package_size": 29895});

  })();

// end include: /var/folders/hx/g36pxlqs1dj0kvmzszq_j3k00000gq/T/tmp2beuh8cn.js
// include: /var/folders/hx/g36pxlqs1dj0kvmzszq_j3k00000gq/T/tmphsd6k0sa.js

    // All the pre-js content up to here must remain later on, we need to run
    // it.
    if (Module['$ww'] || (typeof ENVIRONMENT_IS_PTHREAD != 'undefined' && ENVIRONMENT_IS_PTHREAD)) Module['preRun'] = [];
    var necessaryPreJSTasks = Module['preRun'].slice();
  // end include: /var/folders/hx/g36pxlqs1dj0kvmzszq_j3k00000gq/T/tmphsd6k0sa.js
// include: /var/folders/hx/g36pxlqs1dj0kvmzszq_j3k00000gq/T/tmpxts8fji8.js

    if (!Module['preRun']) throw 'Module.preRun should exist because file support used it; did a pre-js delete it?';
    necessaryPreJSTasks.forEach((task) => {
      if (Module['preRun'].indexOf(task) < 0) throw 'All preRun tasks that exist before user pre-js code should remain after; did you replace Module or modify Module.preRun?';
    });
  // end include: /var/folders/hx/g36pxlqs1dj0kvmzszq_j3k00000gq/T/tmpxts8fji8.js


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
var wasmBinaryFile = 'data:application/octet-stream;base64,AGFzbQEAAAABwgIxYAJ/fwF/YAJ/fwBgA39/fwBgBX9/f39/AX9gAX8Bf2ADf39/AX9gA39+fwF+YAZ/fH9/f38Bf2AEf39/fwBgAX8AYAV/f35/fwBgBX9/f39/AGAFf39/fn4AYAZ/f39/f38AYAABf2ADf3x8AX9gA39+fwF/YAR/f39/AX9gBH9+f38Bf2AAAGAGf39/f39/AX9gB39/f39/f38Bf2ACf38BfWADf399AGADf319AGABfwF8YAF/AX5gAnx/AX9gAXwBfWACfX8Bf2ABfQF9YAF8AXxgAnx/AXxgAn9+AGAFf35+fn4AYAR/fn5/AGACfn4Bf2ADf35+AGAHf39/f39/fwBgAn9/AX5gAn9/AXxgA3x8fwF8YAN+f38Bf2ACfn8Bf2ABfAF+YAR+fn5+AX9gAn98AGACf30AYAJ+fgF8AqwONwNlbnYNX19hc3NlcnRfZmFpbAAIA2VudgRleGl0AAkDZW52KWVtc2NyaXB0ZW5fc2V0X2tleWRvd25fY2FsbGJhY2tfb25fdGhyZWFkAAMDZW52J2Vtc2NyaXB0ZW5fc2V0X2tleXVwX2NhbGxiYWNrX29uX3RocmVhZAADA2VuditlbXNjcmlwdGVuX3NldF9tb3VzZW1vdmVfY2FsbGJhY2tfb25fdGhyZWFkAAMDZW52J2Vtc2NyaXB0ZW5fc2V0X3doZWVsX2NhbGxiYWNrX29uX3RocmVhZAADA2Vudh93Z3B1RGV2aWNlQ3JlYXRlQmluZEdyb3VwTGF5b3V0AAADZW52HndncHVEZXZpY2VDcmVhdGVQaXBlbGluZUxheW91dAAAA2Vudh53Z3B1RGV2aWNlQ3JlYXRlUmVuZGVyUGlwZWxpbmUAAANlbnYkd2dwdVJlbmRlclBpcGVsaW5lR2V0QmluZEdyb3VwTGF5b3V0AAADZW52GXdncHVEZXZpY2VDcmVhdGVCaW5kR3JvdXAAAANlbnYad2dwdUJpbmRHcm91cExheW91dFJlbGVhc2UACQNlbnYZd2dwdVJlbmRlclBpcGVsaW5lUmVsZWFzZQAJA2Vudhl3Z3B1UGlwZWxpbmVMYXlvdXRSZWxlYXNlAAkDZW52F3dncHVTaGFkZXJNb2R1bGVSZWxlYXNlAAkDZW52IHdncHVSZW5kZXJQYXNzRW5jb2RlclNldFBpcGVsaW5lAAEDZW52FHdncHVRdWV1ZVdyaXRlQnVmZmVyAAoDZW52IXdncHVSZW5kZXJQYXNzRW5jb2RlclNldEJpbmRHcm91cAALA2VudiR3Z3B1UmVuZGVyUGFzc0VuY29kZXJTZXRWZXJ0ZXhCdWZmZXIADANlbnYjd2dwdVJlbmRlclBhc3NFbmNvZGVyU2V0SW5kZXhCdWZmZXIADANlbnYgd2dwdVJlbmRlclBhc3NFbmNvZGVyRHJhd0luZGV4ZWQADQNlbnYcd2dwdURldmljZUNyZWF0ZVNoYWRlck1vZHVsZQAAA2VudhZ3Z3B1RGV2aWNlQ3JlYXRlQnVmZmVyAAADZW52HGVtc2NyaXB0ZW5fd2ViZ3B1X2dldF9kZXZpY2UADgNlbnYSd2dwdURldmljZUdldFF1ZXVlAAQDZW52HmVtc2NyaXB0ZW5fcmVxdWVzdF9wb2ludGVybG9jawAAA2VudihlbXNjcmlwdGVuX3NldF9yZXNpemVfY2FsbGJhY2tfb25fdGhyZWFkAAMDZW52H2Vtc2NyaXB0ZW5fZ2V0X2VsZW1lbnRfY3NzX3NpemUABQNlbnYfZW1zY3JpcHRlbl9zZXRfZWxlbWVudF9jc3Nfc2l6ZQAPA2VudhR3Z3B1U3dhcENoYWluUmVsZWFzZQAJA2VudhB3Z3B1UXVldWVSZWxlYXNlAAkDZW52EXdncHVEZXZpY2VSZWxlYXNlAAkDZW52IndncHVTd2FwQ2hhaW5HZXRDdXJyZW50VGV4dHVyZVZpZXcABANlbnYed2dwdURldmljZUNyZWF0ZUNvbW1hbmRFbmNvZGVyAAADZW52IXdncHVDb21tYW5kRW5jb2RlckJlZ2luUmVuZGVyUGFzcwAAA2Vudhh3Z3B1UmVuZGVyUGFzc0VuY29kZXJFbmQACQNlbnYYd2dwdUNvbW1hbmRFbmNvZGVyRmluaXNoAAADZW52D3dncHVRdWV1ZVN1Ym1pdAACA2Vudhx3Z3B1UmVuZGVyUGFzc0VuY29kZXJSZWxlYXNlAAkDZW52GXdncHVDb21tYW5kRW5jb2RlclJlbGVhc2UACQNlbnYYd2dwdUNvbW1hbmRCdWZmZXJSZWxlYXNlAAkDZW52FndncHVUZXh0dXJlVmlld1JlbGVhc2UACQNlbnYYZW1zY3Jp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ASGQESCPESCQEXEhkREgkRFFDQEgAygCWCGSESCSESgCbCGTESADKAIkIZQRQSghlREglBEglRFsIZYRIJMRIJYRaiGXESCXESgCBCGYESADKAIgIZkRQQIhmhEgmREgmhF0IZsRIJgRIJsRaiGcESCcESgCACGdEUEAIZ4RIJ0RIJ4RRyGfEUEBIaARIJ8RIKARcSGhEQJAAkAgoRFFDQAgAygCWCGiESCiESgCbCGjESADKAIkIaQRQSghpREgpBEgpRFsIaYRIKMRIKYRaiGnESCnESgCBCGoESADKAIgIakRQQIhqhEgqREgqhF0IasRIKgRIKsRaiGsESCsESgCACGtESADKAJYIa4RIK4RKAKIASGvESCtESCvEUshsBFBASGxESCwESCxEXEhshEgshFFDQELQX8hsxEgAyCzETYCXAwFCyADKAJYIbQRILQRKAKEASG1ESADKAJYIbYRILYRKAJsIbcRIAMoAiQhuBFBKCG5ESC4ESC5EWwhuhEgtxEguhFqIbsRILsRKAIEIbwRIAMoAiAhvRFBAiG+ESC9ESC+EXQhvxEgvBEgvxFqIcARIMARKAIAIcERQQEhwhEgwREgwhFrIcMRQcABIcQRIMMRIMQRbCHFESC1ESDFEWohxhEgAygCWCHHESDHESgCbCHIESADKAIkIckRQSghyhEgyREgyhFsIcsRIMgRIMsRaiHMESDMESgCBCHNESADKAIgIc4RQQIhzxEgzhEgzxF0IdARIM0RINARaiHRESDRESDGETYCACADKAIgIdIRQQEh0xEg0hEg0xFqIdQRIAMg1BE2AiAMAAsLIAMoAlgh1REg1REoAmwh1hEgAygCJCHXEUEoIdgRINcRINgRbCHZESDWESDZEWoh2hEg2hEoAgwh2xFBACHcESDbESDcEUch3RFBASHeESDdESDeEXEh3xECQCDfEUUNACADKAJYIeARIOARKAJsIeERIAMoAiQh4hFBKCHjESDiESDjEWwh5BEg4REg5BFqIeURIOURKAIMIeYRIAMoAlgh5xEg5xEoAogBIegRIOYRIOgRSyHpEUEBIeoRIOkRIOoRcSHrEQJAIOsRRQ0AQX8h7BEgAyDsETYCXAwECyADKAJYIe0RIO0RKAKEASHuESADKAJYIe8RIO8RKAJsIfARIAMoAiQh8RFBKCHyESDxESDy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IAsgEiAOIBEQ9YCAgAAhEyAHIBM2AhAgBygCECEUQQAhFSAUIBVIIRZBASEXIBYgF3EhGAJAAkAgGEUNACAHKAIQIRkgByAZNgIcDAELQQAhGiAHIBo2AgQCQANAIAcoAgQhGyAHKAIIIRwgHCgCiAEhHSAbIB1JIR5BASEfIB4gH3EhICAgRQ0BIAcoAhghISAHKAIUISIgBygCECEjIAcoAgwhJCAHKAIIISUgJSgChAEhJiAHKAIEISdBwAEhKCAnIChsISkgJiApaiEqICEgIiAjICQgKhCAgYCAACErIAcgKzYCECAHKAIQISxBACEtICwgLUghLkEBIS8gLiAvcSEwAkAgMEUNACAHKAIQITEgByAxNgIcDAMLIAcoAgQhMkEBITMgMiAzaiE0IAcgNDYCBAwACwsgBygCECE1IAcgNTYCHAsgBygCHCE2QSAhNyAHIDdqITggOCSAgICAACA2DwvzAwE0fyOAgICAACEFQSAhBiAFIAZrIQcgBySAgICAACAHIAA2AhggByABNgIUIAcgAjYCECAHIAM2AgwgByAENgIIIAcoAhghCCAHKAIUIQkgBygCECEKIAcoAgwhCyAHKAIIIQxBjAEhDSAMIA1qIQ4gBygCCCEPQZABIRAgDyAQaiERQSAhEiAIIAkgCiALIBIgDiAREPWAgIAAIRMgByATNgIQIAcoAhAhFEEAIRUgFCAVSCEWQQEhFyAWIBdxIRgCQAJAIBhFDQAgBygCECEZIAcgGTYCHAwBC0EAIRogByAaNgIEAkADQCAHKAIEIRsgBygCCCEcIBwoApABIR0gGyAdSSEeQQEhHyAeIB9xISAgIEUNASAHKAIYISEgBygCFCEiIAcoAhAhIyAHKAIMISQgBygCCCElICUoAowBISYgBygCBCEnQQUhKCAnICh0ISkgJiApaiEqICEgIiAjICQgKhCBgYCAACErIAcgKzYCECAHKAIQISxBACEtICwgLUghLkEBIS8gLiAvcSEwAkAgMEUNACAHKAIQITEgByAxNgIcDAMLIAcoAgQhMkEBITMgMiAzaiE0IAcgNDYCBAwACwsgBygCECE1IAcgNTYCHAsgBygCHCE2QSAhNyAHIDdqITggOCSAgICAACA2DwudAwEwfyOAgICAACECQaABIQMgAiADayEEIAQkgICAgAAgBCAANgKY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IEwgSzgCGCADKALMASFNIE0qAgwhTiADKALIASFPIE8gTjgCDCADKALMASFQIFAqAhwhUSADKALIASFSIFIgUTgCHCADKALMASFTIFMqAiAhVCADKALIASFVIFUgVDgCICADKALMASFWIFYqAjAhVyADKALIASFYIFggVzgCMCADKALMASFZIFkqAiQhWiADKALIASFbIFsgWjgCJCADKALMASFcIFwqAjQhXSADKALIASFeIF4gXTgCNCADKALMASFfIF8qAighYCADKALIASFhIGEgYDgCKCADKALMASFiIGIqAjghYyADKALIASFkIGQgYzgCOCADKALMASFlIGUqAiwhZiADKALIASFnIGcgZjgCLCADKALMASFoIGgqAjwhaSADKALIASFqIGogaTgCPEHAACFrIAMga2ohbEEAIW0gbCBtNgIAQgAhbiADIG43AzhBOCFvIAMgb2ohcCBwIXEgAygCRCFyQRwhcyByIHNqIXQgAyBxNgK8ASADIHQ2ArgBIAMoArwBIXUgdSoCACF2IAMoArgBIXcgdyB2OAIAIAMoArwBIXggeCoCBCF5IAMoArgBIXogeiB5OAIEIAMoArwBIXsgeyoCCCF8IAMoArgBIX0gfSB8OAIIQQAhfiB+KAKsmYSAACF/QTAhgAEgAyCAAWohgQEggQEgfzYCACB+KQKkmYSAACGCASADIIIBNwMoQSghgwEgAyCDAWohhAEghAEhhQEgAygCRCGGAUE0IYcBIIYBIIcBaiGIASADIIUBNgK0ASADIIgBNgKwASADKAK0ASGJASCJASoCACGKASADKAKwASGLASCLASCKATgCACADKAK0ASGMASCMASoCBCGNASADKAKwASGOASCOASCNATgCBCADKAK0ASGPASCPASoCCCGQASADKAKwASGRASCRASCQATgCCEEgIZIBIAMgkgFqIZMBQQAhlAEgkwEglAE2AgBCACGVASADIJUBNwMYQRghlgEgAyCWAWohlwEglwEhmAEgAygCRCGZAUEoIZoBIJkBIJoBaiGbASADIJgBNgKsASADIJsBNgKoASADKAKsASGcASCcASoCACGdASADKAKoASGeASCeASCdATgCACADKAKsASGfASCfASoCBCGgASADKAKoASGhASChASCgATgCBCADKAKsASGiASCiASoCCCGjASAD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IOIBEMCBgIAAIAMoAjQh4wEgAygCNCHkAUEEIeUBIOQBIOUBaiHmASADKAI0IecBQRwh6AEg5wEg6AFqIekBIOMBIOYBIOkBEMGBgIAAQYABIeoBIAMg6gFqIesBIOsBJICAgIAADwuLQdACB38BfQF/An0BfwF9AX8CfQh/AX0BfwR9AX8BfQF/BX0BfwF9AX8GfQJ8AX8BfQN8AX0DfwJ9AX8BfQF/AX0Dfwd9C38BfQF/AX0BfwF9AX8EfQF/AX0BfwZ9Bn8BfQJ/AX0CfwF9AX8DfQJ/A30CfwN9An8DfQF/A30BfwN9AX8DfQF/AX0EfwF9AX8CfQF/AX0Dfwd9C38BfQF/AX0BfwF9AX8EfQF/AX0BfwZ9Bn8BfQJ/AX0CfwF9AX8DfQJ/A30CfwN9An8DfQF/A30BfwN9AX8DfQF/AX0LfwF9AX8BfQF/AX0BfwR9AX8BfQF/A30BfwF9AX8EfQJ/AX0BfwF9AX8BfQF/BX0BfwF9AX8DfQF/AX0BfwN9An8BfQF/AX0BfwF9AX8EfQF/AX0BfwR9AX8BfQF/A30CfwF9AX8BfQF/AX0BfwV9AX8BfQF/BH0BfwF9AX8EfQJ/AX0BfwJ9EX8BfQF/AX0BfwF9AX8EfQF/AX0BfwN9AX8BfQF/BH0BfwF9BX8CfgV/AX0CfwF9An8BfQJ/AX0CfwR9An8DfQJ/A30CfwN9An8DfQh/AX0CfwF9An8BfQV/AX0FfwF9AX8BfQF/AX0BfwR9AX8BfQF/BX0HfwN9An8DfQJ/A30CfwJ9B38BfQF/AX0BfwF9AX8EfQF/AX0BfwZ9BH8DfQJ/A30CfwN9C38BfQF/An0CfwF9AX8CfQJ/AX0BfwJ9CX8BfQF/AX0BfwF9AX8FfQF/AX0BfwF9AX8BfQF/BX0BfwF9AX8BfQF/AX0BfwV9BX8BfQJ/AX0CfwF9AX8DfQd/A30CfwN9An8DfQl/AX0BfwJ9An8BfQF/An0CfwF9AX8CfQt/AX0BfwJ9An8BfQF/An0CfwF9AX8CfQp/I4CAgIAAIQFB4AQhAiABIAJrIQMgAySAgICAACADIAA2AmxBsM2EgAAhBCAEKAKAASEFQQAhBiAGIAVrIQcgB7IhCCADKAJsIQkgCSoClAEhCiAIIAqUIQsgAyALOAJoIAQoAoQBIQwgDLIhDSADKAJsIQ4gDioC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kQIgAygC2AEhkgIgkgIqAgghkwIgAygC1AEhlAIglAIqAgQhlQIgkwKMIZYCIJYCIJUClCGXAiCXAiCRApIhmAIgAygC0AEhmQIgmQIgmAI4AgAgAygC2AEhmgIgmgIqAgwhmwIgAygC1AEhnAIgnAIqAgQhnQIgAygC2AEhngIgngIqAgAhnwIgAygC1AEhoAIgoAIqAgghoQIgnwIgoQKUIaICIKICjCGjAiCbAiCdApQhpAIgpAIgowKSIaUCIAMoAtgBIaYCIKYCKgIEIacCIAMoAtQBIagCIKgCKgIMIakCIKcCIKkClCGqAiCqAiClApIhqwIgAygC2AEhrAIgrAIqAgghrQIgAygC1AEhrgIgrgIqAgAhrwIgrQIgrwKUIbACILACIKsCkiGxAiADKALQASGyAiCyAiCxAjgCBCADKALYASGzAiCzAioCDCG0AiADKALUASG1AiC1AioCCCG2AiADKALYASG3AiC3AioCACG4AiADKALUASG5AiC5AioCBCG6AiC4AiC6ApQhuwIgtAIgtgKUIbwCILwCILsCkiG9AiADKALYASG+AiC+AioCBCG/AiADKALUASHAAiDAAioCACHBAiC/AowhwgIgwgIgwQKUIcMCIMMCIL0CkiHEAiADKALYASHFAiDFAioCCCHGAiADKALUASHHAiDHAioCDCHIAiDGAiDIApQhyQIgyQIgxAKSIcoCIAMoAtABIcsCIMsCIMoCOAIIIAMoAtgBIcwCIMwCKgIMIc0CIAMoAtQBIc4CIM4CKgIMIc8CIAMoAtgBIdACINACKgIAIdECIAMoAtQBIdICINICKgIAIdMCINECINMClCHUAiDUAowh1QIgzQIgzwKUIdYCINYCINUCkiHXAiADKALYASHYAiDYAioCBCHZAiADKALUASHaAiDaAioCBCHbAiDZAowh3AIg3AIg2wKUId0CIN0CINcCkiHeAiADKALYASHfAiDfAioCCCHgAiADKALUASHhAiDhAioCCCHiAiDgAowh4wIg4wIg4gKUIeQCIOQCIN4CkiHlAiADKALQASHmAiDmAiDlAjgCDEEAIecCIOcCsiHoAiADIOgCOAIMQQAh6QIg6QKyIeoCIAMg6gI4AhAgAyoCYCHrAiADIOsCOAIUQTAh7AIgAyDsAmoh7QIg7QIh7gJBDCHvAiADIO8C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vgUgvQUgvgVqIb8FIAMoAmwhwAVBBCHBBSDABSDBBWohwgUgAyC8BTYCeCADIL8FNgJ0IAMgwgU2AnAgAygCeCHDBSDDBSoCACHEBSADKAJ0IcUFIMUFKgIAIcYFIMQFIMYFkiHHBSADKAJwIcgFIMgFIMcFOAIAIAMoAnghyQUgyQUqAgQhygUgAygCdCHLBSDLBSoCBCHMBSDKBSDMBZIhzQUgAygCcCHOBSDOBSDNBTgCBCADKAJ4Ic8FIM8FKgIIIdAFIAMoAnQh0QUg0QUqAggh0gUg0AUg0gWSIdMFIAMoAnAh1AUg1AUg0wU4AgggAygCbCHVBSADKAJsIdYFQQQh1wUg1gUg1wVqIdgFIAMoAmwh2QVBHCHaBSDZBSDaBWoh2wUg1QUg2AUg2wUQwYGAgABB4AQh3AUgAyDcBWoh3QUg3QUkgICAgAAPC45KkQMPfwF9AX8CfQl/AX0BfwF9AX8BfQF/BH0BfwF9AX8GfQZ/AX0CfwF9An8BfQF/A30CfwN9An8DfQJ/A30BfwN9B38DfQJ/A30CfwN9AX8CfQd/A30CfwN9An8DfQF/AX0FfwN9An8DfQJ/A30BfwF9B38DfQJ/A30CfwN9AX8BfQd/A30CfwN9An8DfQF/AX0BfwN9AX8DfQF/A30BfwN9AX8DfQF/A30BfwN9AX8DfQF/An0CfwF9An8BfQJ/AX0CfwF9An8BfQJ/AX0BfwF9BX8BfQF/AX0EfwF9An8BfQJ/AX0BfwF9CX8BfQF/AX0BfwF9AX8EfQF/AX0BfwN9AX8BfQF/A30BfwF9AX8BfQF/AX0BfwR9AX8BfQF/A30BfwF9AX8DfQF/AX0BfwF9AX8BfQF/BH0BfwF9AX8DfQF/AX0BfwN9AX8BfQF/AX0BfwF9AX8EfQF/AX0BfwN9AX8BfQF/A30FfwF9An8BfQJ/AX0CfwF9Bn8BfQJ/AX0CfwF9An8BfQF/An0JfwF9AX8BfQF/AX0BfwR9AX8BfQF/Bn0GfwF9An8BfQJ/AX0BfwN9An8DfQJ/A30CfwN9AX8DfQd/A30CfwN9An8DfQF/An0HfwN9An8DfQJ/A30BfwF9BX8DfQJ/A30CfwN9AX8BfQd/A30CfwN9An8DfQF/AX0HfwN9An8DfQJ/A30BfwF9AX8DfQF/A30BfwN9AX8DfQF/A30BfwN9AX8D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Rg0AIABBAEEAIAAoAiQRhYCAgACAgICAABogACgCFA0AQX8hASACRQ0BDAILAkAgACgCBCIBIAAoAggiA0YNACAAIAEgA2usQQEgACgCKBGGgICAAICAgIAAGgtBACEBIABBADYCHCAAQgA3AxAgAEIANwIEIAINAQsgABD5gYCAAAsgAQsFACAAnAt9AQF/QQIhAQJAIABBKxCugoCAAA0AIAAtAABB8gBHIQELIAFBgAFyIAEgAEH4ABCugoCAABsiAUGAgCByIAEgAEHlABCugoCAABsiASABQcAAciAALQAAIgBB8gBGGyIBQYAEciABIABB9wBGGyIBQYAIciABIABB4QBGGwvyAgIDfwF+AkAgAkUNACAAIAE6AAAgACACaiIDQX9qIAE6AAAgAkEDSQ0AIAAgAToAAiAAIAE6AAEgA0F9aiABOgAAIANBfmogAToAACACQQdJDQAgACABOgADIANBfGogAToAACACQQlJDQAgAEEAIABrQQNxIgRqIgMgAUH/AXFBgYKECGwiATYCACADIAIgBGtBfHEiBGoiAkF8aiABNgIAIARBCUkNACADIAE2AgggAyABNgIEIAJBeGogATYCACACQXRqIAE2AgAgBEEZSQ0AIAMgATYCGCADIAE2AhQgAyABNgIQIAMgATYCDCACQXBqIAE2AgAgAkFsaiABNgIAIAJBaGogATYCACACQWRqIAE2AgAgBCADQQRxQRhyIgVrIgJBIEkNACABrUKBgICAEH4hBiADIAVqIQEDQCABIAY3AxggASAGNwMQIAEgBjcDCCABIAY3AwAgAUEgaiEBIAJBYGoiAkEfSw0ACwsgAAsRACAAKAI8IAEgAhCVgoCAAAv/AgEHfyOAgICAAEEgayIDJICAgIAAIAMgACgCHCIENgIQIAAoAhQhBSADIAI2AhwgAyABNgIYIAMgBSAEayIBNgIUIAEgAmohBiADQRBqIQRBAiEHAkACQAJAAkACQCAAKAI8IANBEGpBAiADQQxqELGAgIAAEOKCgIAARQ0AIAQhBQwBCwNAIAYgAygCDCIBRg0CAkAgAUF/Sg0AIAQhBQwECyAEIAEgBCgCBCIISyIJQQN0aiIFIAUoAgAgASAIQQAgCRtrIghqNgIAIARBDEEEIAkbaiIEIAQoAgAgCGs2AgAgBiABayEGIAUhBCAAKAI8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IAFrrFkNACABIASnaiEFCyAAIAU2AmggACADIAAoAiwiBSABa6x8NwN4AkAgASAFSw0AIAFBf2ogAjoAAAsgAgs8ACAAIAE3AwAgACAEQjCIp0GAgAJxIAJCgICAgICAwP//AINCMIincq1CMIYgAkL///////8/g4Q3AwgL5gIBAX8jgICAgABB0ABrIgQkgICAgAACQAJAIANBgIABSA0AIARBIGogASACQgBCgICAgICAgP//ABD8goCAACAEKQMoIQIgBCkDICEBAkAgA0H//wFPDQAgA0GBgH9qIQMMAgsgBEEQaiABIAJCAEKAgICAgICA//8AEPyCgIAAIANB/f8CIANB/f8CSRtBgoB+aiEDIAQpAxghAiAEKQMQIQEMAQsgA0GBgH9KDQAgBEHAAGogASACQgBCgICAgICAgDkQ/IKAgAAgBCkDSCECIAQpA0AhAQJAIANB9IB+TQ0AIANBjf8AaiEDDAELIARBMGogASACQgBCgICAgICAgDkQ/IKAgAAgA0HogX0gA0HogX1LG0Ga/gFqIQMgBCkDOCECIAQpAzAhAQsgBCABIAJCACADQf//AGqtQjCGEPyCgIAAIAAgBCkDCDcDCCAAIAQpAwA3AwAgBEHQAGokgICAgAALSwIBfgJ/IAFC////////P4MhAgJAAkAgAUIwiKdB//8BcSIDQf//AUYNAEEEIQQgAw0BQQJBAyACIACEUBsPCyACIACEUCEECyAEC+cGBAN/An4BfwF+I4CAgIAAQYABayIFJICAgIAAAkACQAJAIAMgBEIAQgAQ8oKAgABFDQAgAyAEEMaCgIAARQ0AIAJCMIinIgZB//8BcSIHQf//AUcNAQsgBUEQaiABIAIgAyAEEPyCgIAAIAUgBSkDECIEIAUpAxgiAyAEIAMQ9IKAgAAgBSkDCCECIAUpAwAhBAwBCwJAIAEgAkL///////////8AgyIIIAMgBEL///////////8AgyIJEPKCgIAAQQBKDQACQCABIAggAyAJEPKCgIAARQ0AIAEhBAwCCyAFQfAAaiABIAJCAEIAEPyCgIAAIAUpA3ghAiAFKQNwIQQMAQsgBEIwiKdB//8BcSEKAkACQCAHRQ0AIAEhBAwBCyAFQeAAaiABIAhCAEKAgICAgIDAu8AAEPyCgIAAIAUpA2giCEIwiKdBiH9qIQcgBSkDYCEECwJAIAoNACAFQdAAaiADIAlCAEKAgICAgIDAu8AAEPyCgIAAIAUpA1giCUIwiKdBiH9qIQogBSkDUCEDCyAJQv///////z+DQoCAgICAgMAAhCELIAhC////////P4NCgICAgICAwACEIQgCQCAHIApMDQADQAJAAkAgCCALfSAEIANUrX0iCUIAUw0AAkAgCSAEIAN9IgSEQgBSDQAgBUEgaiABIAJCAEIAEPyCgIAAIAUpAyghAiAFKQMgIQQMBQsgCUIBhiAEQj+IhCEIDAELIAhCAYYgBEI/iIQhCAsgBEIBhiEEIAdBf2oiByAKSg0ACyAKIQcLAkACQCAIIAt9IAQgA1StfSIJQgBZDQAgCCEJDAELIAkgBCADfSIEhEIAUg0AIAVBMGogASACQgBCABD8goCAACAFKQM4IQIgBSkDMCEEDAELAkAgCUL///////8/Vg0AA0AgBEI/iCEDIAdBf2ohByAEQgGGIQQgAyAJQgGGhCIJQoCAgICAgMAAVA0ACwsgBkGAgAJxIQoCQCAHQQBKDQAgBUHAAGogBCAJQv///////z+DIAdB+ABqIApyrUIwhoRCAEKAgICAgIDAwz8Q/IKAgAAgBSkDSCECIAUpA0AhBAwBCyAJQv///////z+DIAcgCnKtQjCGhCECCyAAIAQ3AwAgACACNwMIIAVBgAFqJICAgIAACxwAIAAgAkL///////////8AgzcDCCAAIAE3AwALzwkEAX8BfgV/AX4jgICAgABBMGsiBCSAgICAAEIAIQUCQAJAIAJBAksNACACQQJ0IgJBnMKEgABqKAIAIQYgAkGQwoSAAGooAgAhBwNAAkACQCABKAIEIgIgASgCaEYNACABIAJBAWo2AgQgAi0AACECDAELIAEQw4KAgAAhAgsgAhDKgoCAAA0AC0EBIQgCQAJAIAJBVWoOAwABAAELQX9BASACQS1GGyEIAkAgASgCBCICIAEoAmhGDQAgASACQQFqNgIEIAItAAAhAgwBCyABEMOCgIAAIQILQQAhCQJAAkACQCACQV9xQckARw0AA0AgCUEHRg0CAkACQCABKAIEIgIgASgCaEYNACABIAJBAWo2AgQgAi0AACECDAELIAEQw4KAgAAhAgsgCUGLgISAAGohCiAJQQFqIQkgAkEgciAKLAAARg0ACwsCQCAJ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C74EAAJAAkACQAJAAkACQAJAAkACQAJAAkACQAJAAkACQAJAAkACQAJAIAFBd2oOEgABAgUDBAYHCAkKCwwNDg8QERILIAIgAigCACIBQQRqNgIAIAAgASgCADYCAA8LIAIgAigCACIBQQRqNgIAIAAgATQCADcDAA8LIAIgAigCACIBQQRqNgIAIAAgATUCADcDAA8LIAIgAigCACIBQQRqNgIAIAAgATQCADcDAA8LIAIgAigCACIBQQRqNgIAIAAgATUCADcDAA8LIAIgAigCAEEHakF4cSIBQQhqNgIAIAAgASkDADcDAA8LIAIgAigCACIBQQRqNgIAIAAgATIBADcDAA8LIAIgAigCACIBQQRqNgIAIAAgATMBADcDAA8LIAIgAigCACIBQQRqNgIAIAAgATAAADcDAA8LIAIgAigCACIBQQRqNgIAIAAgATEAADcDAA8LIAIgAigCAEEHakF4cSIBQQhqNgIAIAAgASkDADcDAA8LIAIgAigCACIBQQRqNgIAIAAgATUCADcDAA8LIAIgAigCAEEHakF4cSIBQQhqNgIAIAAgASkDADcDAA8LIAIgAigCAEEHakF4cSIBQQhqNgIAIAAgASkDADcDAA8LIAIgAigCACIBQQRqNgIAIAAgATQCADcDAA8LIAIgAigCACIBQQRqNgIAIAAgATUCADcDAA8LIAIgAigCAEEHakF4cSIBQQhqNgIAIAAgASsDADkDAA8LIAAgAiADEYGAgIAAgICAgAALC0ABAX8CQCAAUA0AA0AgAUF/aiIBIACnQQ9xQYDGhIAAai0AACACcjoAACAAQg9WIQMgAEIEiCEAIAMNAAsLIAELNgEBfwJAIABQDQADQCABQX9qIgEgAKdBB3FBMHI6AAAgAEIHViECIABCA4ghACACDQALCyABC4oBAgF+A38CQAJAIABCgICAgBBaDQAgACECDAELA0AgAUF/aiIBIAAgAEIKgCICQgp+fadBMHI6AAAgAEL/////nwFWIQMgAiEAIAMNAAsLAkAgAlANACACpyEDA0AgAUF/aiIBIAMgA0EKbiIEQQpsa0EwcjoAACADQQlLIQUgBCEDIAUNAAsLIAELhAEBAX8jgICAgABBgAJrIgUkgICAgAACQCACIANMDQAgBEGAwARxDQAgBSABIAIgA2siA0GAAiADQYACSSIC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ASAESQ0BIAEgADYCDCAFIAA2AgggAEEANgIYIAAgBTYCDCAAIAE2AggLDwsQ5oKAgAAAC2sCAX8BfgJAAkAgAA0AQQAhAgwBCyAArSABrX4iA6chAiABIAByQYCABEkNAEF/IAIgA0IgiKdBAEcbIQILAkAgAhDngoCAACIARQ0AIABBfGotAABBA3FFDQAgAEEAIAIQ/4GAgAAaCyAACwcAPwBBEHQLYQECf0EAKAKszYSAACIBIABBB2pBeHEiAmohAAJAAkACQCACRQ0AIAAgAU0NAQsgABDugoCAAE0NASAAELaAgIAADQELEPCBgIAAQTA2AgBBfw8LQQAgADYCrM2EgAAgAQv6CgcBfwF+AX8CfgF/AX4BfyOAgICAAEHwAGsiBSSAgICAACAEQv///////////wCDIQYCQAJAAkAgAVAiByACQv///////////wCDIghCgICAgICAwICAf3xCgICAgICAwICAf1QgCFAbDQAgA0IAUiAGQoCAgICAgMCAgH98IglCgICAgICAwICAf1YgCUKAgICAgIDAgIB/URsNAQsCQCAHIAhCgICAgICAwP//AFQgCEKAgICAgIDA//8AURsNACACQoCAgICAgCCEIQQgASEDDAILAkAgA1AgBkKAgICAgIDA//8AVCAGQoCAgICAgMD//wBRGw0AIARCgICAgICAIIQhBAwCCwJAIAEgCEKAgICAgIDA//8AhYRCAFINAEKAgICAgIDg//8AIAIgAyABhSAEIAKFQoCAgICAgICAgH+FhFAiBxshBEIAIAEgBxshAwwCCyADIAZCgICAgICAwP//AIWEUA0BAkAgASAIhEIAUg0AIAMgBoRCAFINAiADIAGDIQMgBCACgyEEDAILIAMgBoRQRQ0AIAEhAyACIQQMAQsgAyABIAMgAVYgBiAIViAGIAhRGyIKGyEGIAQgAiAKGyIJQv///////z+DIQggAiAEIAobIgtCMIinQf//AXEhDAJAIAlCMIinQf//AXEiBw0AIAVB4ABqIAYgCCAGIAggCFAiBxt5IAdBBnStfKciB0FxahDxgoCAAEEQIAdrIQcgBSkDaCEIIAUpA2AhBgsgASADIAobIQMgC0L///////8/gyEBAkAgDA0AIAVB0ABqIAMgASADIAEgAVAiCht5IApBBnStfKciCkFxahDxgoCAAEEQIAprIQwgBSkDWCEBIAUpA1AhAwsgAUIDhiADQj2IhEKAgICAgICABIQhASAIQgOGIAZCPYiEIQsgA0IDhiEIIAQgAoUhAwJAIAcgDEYNAAJAIAcgDGsiCkH/AE0NAEIAIQFCASEIDAELIAVBwABqIAggAUGAASAKaxDxgoCAACAFQTBqIAggASAKEPuCgIAAIAUpAzAgBSkDQCAFKQNIhEIAUq2EIQggBSkDOCEBCyALQoCAgICAgIAEhCELIAZCA4YhBgJAAkAgA0J/VQ0AQgAhA0IAIQQgBiAIhSALIAGFhFANAiAGIAh9IQIgCyABfSAGIAhUrX0iBEL/////////A1YNASAFQSBqIAIgBCACIAQgBFAiCht5IApBBnStfKdBdGoiChDxgoCAACAHIAprIQcgBSkDKCEEIAUpAyAhAgwBCyABIAt8IAggBnwiAiAIVK18IgRCgICAgICAgAiDUA0AIAJCAYggBEI/hoQgCEIBg4QhAiAHQQFqIQcgBEIBiCEECyAJQoCAgICAgICAgH+DIQgCQCAHQf//AUgNACAIQoCAgICAgMD//wCEIQRCACEDDAELQQAhCgJAAkAgB0EATA0AIAchCgwBCyAFQRBqIAIgBCAHQf8AahDxgoCAACAFIAIgBEEBIAdrEPuCgIAAIAUpAwAgBSkDECAFKQMYhEIAUq2EIQIgBSkDCCEECyACQgOIIARCPYaEIQMgCq1CMIYgBEIDiEL///////8/g4QgCIQhBCACp0EHcSEHAkACQAJAAkACQBD5goCAAA4DAAECAwsCQCAHQQRGDQAgBCADIAdBBEutfCIIIANUrXwhBCAIIQMMAwsgBCADIANCAYN8IgggA1StfCEEIAghAwwDCyAEIAMgCEIAUiAHQQBHca18IgggA1StfCEEIAghAwwBCyAEIAMgCFAgB0EAR3GtfCIIIANUrXwhBCAIIQMLIAdFDQELEPqCgIAAGgsgACADNwMAIAAgBDcDCCAFQfAAaiSAgICAAAtTAQF+AkACQCADQcAAcUUNACABIANBQGqthiECQgAhAQwBCyADRQ0AIAFBwAAgA2utiCACIAOtIgSGhCECIAEgBIYhAQsgACABNwMAIAAgAjcDCAvmAQIBfwJ+QQEhBAJAIABCAFIgAUL///////////8AgyIFQoCAgICAgMD//wBWIAVCgICAgICAwP//AFEbDQAgAkIAUiADQv///////////wCDIgZCgICAgICAwP//AFYgBkKAgICAgIDA//8AURsNAAJAIAIgAIQgBiAFhIRQRQ0AQQAPCwJAIAMgAYNCAFMNAAJAIAAgAlQgASADUyABIANRG0UNAEF/DwsgACAChSABIAOFhEIAUg8LAkAgACACViABIANVIAEgA1EbRQ0AQX8PCyAAIAKFIAEgA4WEQgBSIQQLIAQL2AECAX8CfkF/IQQCQCAAQgBSIAFC////////////AIMiBUKAgICAgIDA//8AViAFQoCAgICAgMD//wBRGw0AIAJCAFIgA0L///////////8AgyIGQoCAgICAgMD//wBWIAZCgICAgICAwP//AFEbDQACQCACIACEIAYgBYSEUEUNAEEADwsCQCADIAGDQgBTDQAgACACVCABIANTIAEgA1EbDQEgACAChSABIAOFhEIAUg8LIAAgAlYgASADVSABIANRGw0AIAAgAoUgASADhYRCAFIhBAsgBAvBEAYBfwN+A38BfgF/C34jgICAgABB0AJrIgUkgICAgAAgBEL///////8/gyEGIAJC////////P4MhByAEIAKFQoCAgICAgICAgH+DIQggBEIwiKdB//8BcSEJAkACQAJAIAJCMIinQf//AXEiCkGBgH5qQYKAfkkNAEEAIQsgCUGBgH5qQYGAfksNAQsCQCABUCACQv///////////wCDIgxCgICAgICAwP//AFQgDEKAgICAgIDA//8AURsNACACQoCAgICAgCCEIQgMAgsCQCADUCAEQv///////////wCDIgJCgICAgICAwP//AFQgAkKAgICAgIDA//8AURsNACAEQoCAgICAgCCEIQggAyEBDAILAkAgASAMQoCAgICAgMD//wCFhEIAUg0AAkAgAyACQoCAgICAgMD//wCFhFBFDQBCACEBQoCAgICAgOD//wAhCAwDCyAIQoCAgICAgMD//wCEIQhCACEBDAILAkAgAyACQoCAgICAgMD//wCFhEIAUg0AQgAhAQwCCwJAIAEgDIRCAFINAEKAgICAgIDg//8AIAggAyAChFAbIQhCACEBDAILAkAgAyAChEIAUg0AIAhCgICAgICAwP//AIQhCEIAIQEMAgtBACELAkAgDEL///////8/Vg0AIAVB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QYz4ACAHa60hBSACKQMAIQQLIAAgBDcDACAAIAVCMIYgA0KAgICAgICAgIB/g4QgBoQ3AwggAkEQaiSAgICAAAvqAQIFfwJ+I4CAgIAAQRBrIgIkgICAgAAgAbwiA0H///8DcSEEAkACQCADQRd2IgVB/wFxIgZFDQACQCAGQf8BRg0AIAStQhmGIQcgBUH/AXFBgP8AaiEEQgAhCAwCCyAErUIZhiEHQgAhCEH//wEhBAwBCwJAIAQNAEIAIQhBACEEQgAhBwwBCyACIAStQgAgBGciBEHRAGoQ8YKAgABBif8AIARrIQQgAikDCEKAgICAgIDAAIUhByACKQMAIQgLIAAgCDcDACAAIAStQjCGIANBH3atQj+GhCAHhDcDCCACQRBqJICAgIAAC5sBAwF/An4BfyOAgICAAEEQayICJICAgIAAAkACQCABDQBCACEDQgAhBAwBCyACIAEgAUEfdSIFcyAFayIFrUIAIAVnIgVB0QBqEPGCgIAAIAIpAwhCgICAgICAwACFQZ6AASAFa61CMIZ8IAFBgICAgHhxrUIghoQhBCACKQMAIQMLIAAgAzcDACAAIAQ3AwggAkEQaiSAgICAAAuBAQIBfwJ+I4CAgIAAQRBrIgIkgICAgAACQAJAIAENAEIAIQNCACEEDAELIAIgAa1CAEHwACABZyIBQR9zaxDxgoCAACACKQMIQoCAgICAgMAAhUGegAEgAWutQjCGfCEEIAIpAwAhAwsgACADNwMAIAAgBDcDCCACQRBqJICAgIAACwQAQQALBABBAAtTAQF+AkACQCADQcAAcUUNACACIANBQGqtiCEBQgAhAgwBCyADRQ0AIAJBwAAgA2uthiABIAOtIgSIhCEBIAIgBIghAgsgACABNwMAIAAgAjcDCAujCwYBfwR+A38BfgF/Cn4jgICAgABB4ABrIgUkgICAgAAgBEL///////8/gyEGIAQgAoVCgICAgICAgICAf4MhByACQv///////z+DIghCIIghCSAEQjCIp0H//wFxIQoCQAJAAkAgAkIwiKdB//8BcSILQYGAfmpBgoB+SQ0AQQAhDCAKQYGAfmpBgYB+Sw0BCwJAIAFQIAJC////////////AIMiDUKAgICAgIDA//8AVCANQoCAgICAgMD//wBRGw0AIAJCgICAgICAIIQhBwwCCwJAIANQIARC////////////AIMiAkKAgICAgIDA//8AVCACQoCAgICAgMD//wBRGw0AIARCgICAgICAIIQhByADIQEMAgsCQCABIA1CgICAgICAwP//AIWEQgBSDQACQCADIAKEUEUNAEKAgICAgIDg//8AIQdCACEBDAMLIAdCgICAgICAwP//AIQhB0IAIQEMAgsCQCADIAJCgICAgICAwP//AIWEQgBSDQAgASANhCECQgAhAQJAIAJQRQ0AQoCAgICAgOD//wAhBwwDCyAHQoCAgICAgMD//wCEIQcMAgsCQCABIA2EQgBSDQBCACEBDAILAkAgAyAChEIAUg0AQgAhAQwCC0EAIQwCQCANQv///////z9WDQAgBUHQAGogASAIIAEgCCAIUCIMG3kgDEEGdK18pyIMQXFqEPGCgIAAQRAgDGshDCAFKQNYIghCIIghCSAFKQNQIQELIAJC////////P1YNACAFQcAAaiADIAYgAyAGIAZQIg4beSAOQQZ0rXynIg5BcWoQ8YKAgAAgDCAOa0EQaiEMIAUpA0ghBiAFKQNAIQMLIANCD4YiDUKAgP7/D4MiAiABQiCIIgR+Ig8gDUIgiCINIAFC/////w+DIgF+fCIQQiCGIhEgAiABfnwiEiARVK0gAiAIQv////8PgyIIfiITIA0gBH58IhEgA0IxiCAGQg+GIhSEQv////8PgyIDIAF+fCIVIBBCIIggECAPVK1CIIaEfCIQIAIgCUKAgASEIgZ+IhYgDSAIfnwiCSAUQiCIQoCAgIAIhCICIAF+fCIPIAMgBH58IhRCIIZ8Ihd8IQEgCyAKaiAMakGBgH9qIQoCQAJAIAIgBH4iGCANIAZ+fCIEIBhUrSAEIAMgCH58Ig0gBFStfCACIAZ+fCANIBEgE1StIBUgEVStfHwiBCANVK18IAMgBn4iAyACIAh+fCICIANUrUIghiACQiCIhHwgBCACQiCGfCICIARUrXwgAiAUQiCIIAkgFlStIA8gCVStfCAUIA9UrXxCIIaEfCIEIAJUrXwgBCAQIBVUrSAXIBBUrXx8IgIgBFStfCIEQoCAgICAgMAAg1ANACAKQQFqIQoMAQsgEkI/iCEDIARCAYYgAkI/iIQhBCACQgGGIAFCP4iEIQIgEkIBhiESIAMgAUIBhoQhAQsCQCAKQf//AUgNACAHQoCAgICAgMD//wCEIQdCACEBDAELAkACQCAKQQBKDQACQEEBIAprIgtB/wBLDQAgBUEwaiASIAEgCkH/AGoiChDxgoCAACAFQSBqIAIgBCAKEPGCgIAAIAVBEGogEiABIAsQ+4KAgAAgBSACIAQgCxD7goCAACAFKQMgIAUpAxCEIAUpAzAgBSkDOIRCAFKthCESIAUpAyggBSkDGIQhASAFKQMIIQQgBSkDACECDAILQgAhAQwCCyAKrUIwhiAEQv///////z+DhCEECyAEIAeEIQcCQCASUCABQn9VIAFCgICAgICAgICAf1EbDQAgByACQgF8IgFQrXwhBwwBCwJAIBIgAUKAgICAgICAgIB/hYRCAFENACACIQEMAQsgByACIAJCAYN8IgEgAlStfCEHCyAAIAE3AwAgACAHNwMIIAVB4ABqJICAgIAAC3UBAX4gACAEIAF+IAIgA358IANCIIgiAiABQiCIIgR+fCADQv////8PgyIDIAFC/////w+DIgF+IgVCIIggAyAEfnwiA0IgiHwgA0L/////D4MgAiABfnwiAUIgiHw3AwggACABQiCGIAVC/////w+DhDcDAAtUAQF/I4CAgIAAQRBrIgUkgICAgAAgBSABIAIgAyAEQoCAgICAgICAgH+FEPCCgIAAIAUpAwAhBCAAIAUpAwg3AwggACAENwMAIAVBEGokgICAgAALmwQDAX8CfgR/I4CAgIAAQSBrIgIkgICAgAAgAUL///////8/gyEDAkACQCABQjCIQv//AYMiBKciBUH/h39qQf0PSw0AIABCPIggA0IEhoQhAyAFQYCIf2qtIQQCQAJAIABC//////////8PgyIAQoGAgICAgICACFQNACADQgF8IQMMAQsgAEKAgICAgICAgAhSDQAgA0IBgyADfCEDC0IAIAMgA0L/////////B1YiBRshACAFrSAEfCEDDAELAkAgACADhFANACAEQv//AVINACAAQjyIIANCBIaEQoCAgICAgIAEhCEAQv8PIQMMAQsCQCAFQf6HAU0NAEL/DyEDQgAhAAwBCwJAQYD4AEGB+AAgBFAiBhsiByAFayIIQfAATA0AQgAhAEIAIQMMAQsgAkEQaiAAIAMgA0KAgICAgIDAAIQgBhsiA0GAASAIaxDxgoCAACACIAAgAyAIEPuCgIAAIAIpAwAiA0I8iCACKQMIQgSGhCEAAkACQCADQv//////////D4MgByAFRyACKQMQIAIpAxiEQgBSca2EIgNCgYCAgICAgIAIVA0AIABCAXwhAAwBCyADQoCAgICAgICACFINACAAQgGDIAB8IQALIABCgICAgICAgAiFIAAgAEL/////////B1YiBRshACAFrSEDCyACQSBqJICAgIAAIANCNIYgAUKAgICAgICAgIB/g4QgAIS/CycAAkAgAEUNAEHIhYSAAEGwiYSAAEEYQduUhIAAEICAgIAAAAtBAQsCAAsKACAAJICAgIAACxoBAn8jgICAgAAgAGtBcHEiASSAgICAACABCwgAI4CAgIAACyAAQYCAhIAAJIKAgIAAQYCAgIAAQQ9qQXBxJIGAgIAACw8AI4CAgIAAI4GAgIAAawsIACOCgICAAAsIACOBgICAAAsLwU0CAEGAgAQLkEZpbnRlbnNpdHkAaW5maW5pdHkAQmluZCBncm91cCBsaXN0IGF0IGZ1bGwgY2FwYWNpdHkAU2NlbmUgbWVzaCBsaXN0IHJlYWNoZWQgZnVsbCBjYXBhY2l0eQBDb3VsZG4ndCByZWFkIGVudGlyZSBmaWxlIGludG8gbWVtb3J5AENvdWxkbid0IGFsbG9jYXRlIG1lbW9yeQBLSFJfbWF0ZXJpYWxzX2FuaXNvdHJvcHkAbWF0cml4AGluZGV4AG1heAAtKyAgIDBYMHgALTBYKzBYIDBYLTB4KzB4IDB4AGJ1ZmZlclZpZXcAeWZvdgBLSFJfdGV4dHVyZV9iYXNpc3UAb3V0cHV0AGlucHV0AHNwb3QAY291bnQAcG9pbnQAS0hSX21hdGVyaWFsc191bmxpdABjb3B5cmlnaHQAbGlnaHQAYXNzZXQAb2Zmc2V0AGJ5dGVPZmZzZXQAdGFyZ2V0AEtIUl9tYXRlcmlhbHNfY2xlYXJjb2F0AGJ1ZmZlclZpZXdzAGpvaW50cwBLSFJfbWF0ZXJpYWxzX3ZhcmlhbnRzAGxpZ2h0cwB3ZWlnaHRzAHRhcmdldHMAS0hSX21hdGVyaWFsc19wYnJTcGVjdWxhckdsb3NzaW5lc3MAcGJyTWV0YWxsaWNSb3VnaG5lc3MAYWNjZXNzb3JzAHNhbXBsZXJzAGJ1ZmZlcnMAYW5pbWF0aW9ucwBleHRlbnNpb25zAHNraW5zAGNoYW5uZWxzAG1hdGVyaWFscwBtYXBwaW5ncwBwcmltaXRpdmVzAHZhbHVlcwBhdHRyaWJ1dGVzAHRleHR1cmVzAHNjZW5lcwB0YXJnZXROYW1lcwBtZXNoZXMAaW1hZ2VzAG5vZGVzAGludmVyc2VCaW5kTWF0cmljZXMAaW5kaWNlcwBjYW52YXMAZXh0cmFzAGNhbWVyYXMAZGVzY3JpcHRvciA9PSBudWxscHRyAGNsZWFyY29hdEZhY3RvcgB0aGlja25lc3NGYWN0b3IAZ2xvc3NpbmVzc0ZhY3RvcgByb3VnaG5lc3NGYWN0b3IAY2xlYXJjb2F0Um91Z2huZXNzRmFjdG9yAHNoZWVuUm91Z2huZXNzRmFjdG9yAHNwZWN1bGFyQ29sb3JGYWN0b3IAZGlmZnVzZVRyYW5zbWlzc2lvbkNvbG9yRmFjdG9yAHNoZWVuQ29sb3JGYWN0b3IAYmFzZUNvbG9yRmFjdG9yAHNwZWN1bGFyRmFjdG9yAHRyYW5zbWlzc2lvbkZhY3RvcgBkaWZmdXNlVHJhbnNtaXNzaW9uRmFjdG9yAGVtaXNzaXZlRmFjdG9yAGRpZmZ1c2VGYWN0b3IAaXJpZGVzY2VuY2VGYWN0b3IAbWV0YWxsaWNGYWN0b3IAZ2VuZXJhdG9yAGNvbG9yAGF0dGVudWF0aW9uQ29sb3IAS0hSX21hdGVyaWFsc19pb3IAaXJpZGVzY2VuY2VJb3IAZmlsdGVyAG1pbkZpbHRlcgBtYWdGaWx0ZXIAc2FtcGxlcgBidWZmZXIAU2hhZGVyAEtIUl9tYXRlcmlhbHNfc3BlY3VsYXIAemZhcgB6bmVhcgAvZW1zZGsvZW1zY3JpcHRlbi9zeXN0ZW0vbGliL3dlYmdwdS93ZWJncHUuY3BwAEVYVF90ZXh0dXJlX3dlYnAAYXNwZWN0UmF0aW8Ac2tlbGV0b24Acm90YXRpb24AYW5pc290cm9weVJvdGF0aW9uAHRyYW5zbGF0aW9uAGludGVycG9sYXRpb24AS0hSX21hdGVyaWFsc190cmFuc21pc3Npb24AS0hSX21hdGVyaWFsc19kaWZmdXNlX3RyYW5zbWlzc2lvbgBFWFRfbWVzaG9wdF9jb21wcmVzc2lvbgBLSFJfZHJhY29fbWVzaF9jb21wcmVzc2lvbgB2ZXJzaW9uAEtIUl9tYXRlcmlhbHNfZGlzcGVyc2lvbgBtaW5WZXJzaW9uAG1pbgBza2luAHZzX21haW4AZnNfbWFpbgBjaGlsZHJlbgBLSFJfbWF0ZXJpYWxzX3NoZWVuAG5hbgBpcmlkZXNjZW5jZVRoaWNrbmVzc01heGltdW0AaXJpZGVzY2VuY2VUaGlja25lc3NNaW5pbXVtAEtIUl90ZXh0dXJlX3RyYW5zZm9ybQAuL3J1bnRpbWUvYXNzZXRzL3NoYWRlci9zaGFkZXIuZGVmYXVsdC53Z3NsAC4vcnVudGltZS9hc3NldHMvc2hhZGVyL3NoYWRlci5wYnIud2dzbAAuL3J1bnRpbWUvYXNzZXRzL3NoYWRlci9zaGFkZXIuZ3JpZC53Z3NsAEtIUl9saWdodHNfcHVuY3R1YWwAZGlyZWN0aW9uYWwAbWF0ZXJpYWwAdXJpAEtIUl9tYXRlcmlhbHNfZW1pc3NpdmVfc3RyZW5ndGgAYW5pc290cm9weVN0cmVuZ3RoAGVtaXNzaXZlU3RyZW5ndGgAYnl0ZUxlbmd0aABwYXRoAG1lc2gARVhUX21lc2hfZ3B1X2luc3RhbmNpbmcAeW1hZwB4bWFnAC4vcmVzb3VyY2VzL2Fzc2V0cy9nbHRmL2ljby5nbHRmAGluZgBhbHBoYUN1dG9mZgBwZXJzcGVjdGl2ZQBTaGFkZXIgaGFzIG5vIGRldmljZSBvciBxdWV1ZQBNZXNoIGhhcyBubyBkZXZpY2Ugb3IgcXVldWUAc3BhcnNlAGFuaXNvdHJvcHlUZXh0dXJlAGNsZWFyY29hdFRleHR1cmUAdGhpY2tuZXNzVGV4dHVyZQBpcmlkZXNjZW5jZVRoaWNrbmVzc1RleHR1cmUAc3BlY3VsYXJHbG9zc2luZXNzVGV4dHVyZQBjbGVhcmNvYXRSb3VnaG5lc3NUZXh0dXJlAHNoZWVuUm91Z2huZXNzVGV4dHVyZQBtZXRhbGxpY1JvdWdobmVzc1RleHR1cmUAc3BlY3VsYXJDb2xvclRleHR1cmUAZGlmZnVzZVRyYW5zbWlzc2lvbkNvbG9yVGV4dHVyZQBzaGVlbkNvbG9yVGV4dHVyZQBiYXNlQ29sb3JUZXh0dXJlAHNwZWN1bGFyVGV4dHVyZQBvY2NsdXNpb25UZXh0dXJlAHRyYW5zbWlzc2lvblRleHR1cmUAZGlmZnVzZVRyYW5zbWlzc2lvblRleHR1cmUAbm9ybWFsVGV4dHVyZQBjbGVhcmNvYXROb3JtYWxUZXh0dXJlAGVtaXNzaXZlVGV4dHVyZQBkaWZmdXNlVGV4dHVyZQBpcmlkZXNjZW5jZVRleHR1cmUAdHlwZQBjb21wb25lbnRUeXBlAG1pbWVUeXBlAHNjZW5lAEtIUl9tYXRlcmlhbHNfdm9sdW1lAG5hbWUAb3V0ZXJDb25lQW5nbGUAaW5uZXJDb25lQW5nbGUAc2NhbGUAcmFuZ2UAbm9kZQBtb2RlAGFscGhhTW9kZQBieXRlU3RyaWRlAHNvdXJjZQBLSFJfbWF0ZXJpYWxzX2lyaWRlc2NlbmNlAHdncHVDcmVhdGVJbnN0YW5jZQBhdHRlbnVhdGlvbkRpc3RhbmNlAGN1YmUAdGV4Q29vcmQAZ3JpZABub3JtYWxpemVkAGV4dGVuc2lvbnNVc2VkAGV4dGVuc2lvbnNSZXF1aXJlZAB1bmRlZmluZWQAZG91YmxlU2lkZWQAb3J0aG9ncmFwaGljAHJiAHJ3YQBjYW1lcmEAd3JhcFQAVEFOR0VOVAB3cmFwUwBKT0lOVFMAV0VJR0hUUwBBVFRSSUJVVEVTAFRSSUFOR0xFUwBJTkRJQ0VTAENPTE9SAFNDQUxBUgBMSU5FQVIAU1RFUABQT1NJVElPTgBRVUFURVJOSU9OAE5BTgBPQ1RBSEVEUkFMAE5PUk1BTABFWFBPTkVOVElBTABNQVNLAElORgBPUEFRVUUATk9ORQBDVUJJQ1NQTElORQBURVhDT09SRABCTEVORABkYXRhOgBNQVQ0AFZFQzQAO2Jhc2U2NABNQVQzAFZFQzMATUFUMgBWRUMyADovLwAuAChudWxsKQBHTFRGIGxvYWRpbmcgYWJvcnRlZCwgb3V0IG9mIG1lbW9yeQoARmFpbGVkIHRvIGV4cGFuZCBtZXNoIGxpc3QKAEdMVEYgbG9hZGluZyBhYm9ydGVkLCB1bmhhbmRlZCBlcnJvcgoAQ291bGRuJ3QgbG9hZCBmaWxlCgBHTFRGIGZpbGUgbm90IGZvdW5kCgBleHBhbmQKAFdBU00gSU5JVAoASW52YWxpZCBHTFRGIEpTT04KAAAAAAAAAAAAAIA/AAAAAAAAAAAAAIA/AAAA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AD2TsQC9fAsApFHcACfdYwBp4d0AmpQZAKgplQBozigACe20AESfIABOmMoAcIJjAH58IwAPuTIAp/WOABRW5wAh8QgAtZ0qAG9+TQClGVEAtfmrAILf1gCW3WEAFjYCAMQ6nwCDoqEAcu1tADmNegCCuKkAazJcAEYnWwAANO0A0gB3APz0VQABWU0A4HGAAAAAAAAAAAAAAAAAQPsh+T8AAAAALUR0PgAAAICYRvg8AAAAYFHMeDsAAACAgxvwOQAAAEAgJXo4AAAAgCKC4zYAAAAAHfNpNYAlAQBObyBlcnJvciBpbmZvcm1hdGlvbgBJbGxlZ2FsIGJ5dGUgc2VxdWVuY2UARG9tYWluIGVycm9yAFJlc3VsdCBub3QgcmVwcmVzZW50YWJsZQBOb3QgYSB0dHkAUGVybWlzc2lvbiBkZW5pZWQAT3BlcmF0aW9uIG5vdCBwZXJtaXR0ZWQATm8gc3VjaCBmaWxlIG9yIGRpcmVjdG9yeQBObyBzdWNoIHByb2Nlc3MARmlsZSBleGlzdHMAVmFsdWUgdG9vIGxhcmdlIGZvciBkYXRhIHR5cGUATm8gc3BhY2UgbGVmdCBvbiBkZXZpY2UAT3V0IG9mIG1lbW9yeQBSZXNvdXJjZSBidXN5AEludGVycnVwdGVkIHN5c3RlbSBjYWxsAFJlc291cmNlIHRlbXBvcmFyaWx5IHVuYXZhaWxhYmxlAEludmFsaWQgc2VlawBDcm9zcy1kZXZpY2UgbGluawBSZWFkLW9ubHkgZmlsZSBzeXN0ZW0ARGlyZWN0b3J5IG5vdCBlbXB0eQBDb25uZWN0aW9uIHJlc2V0IGJ5IHBlZXIAT3BlcmF0aW9uIHRpbWVkIG91dABDb25uZWN0aW9uIHJlZnVzZWQASG9zdCBpcyBkb3duAEhvc3QgaXMgdW5yZWFjaGFibGUAQWRkcmVzcyBpbiB1c2UAQnJva2VuIHBpcGUASS9PIGVycm9yAE5vIHN1Y2ggZGV2aWNlIG9yIGFkZHJlc3MAQmxvY2sgZGV2aWNlIHJlcXVpcmVkAE5vIHN1Y2ggZGV2aWNlAE5vdCBhIGRpcmVjdG9yeQBJcyBhIGRpcmVjdG9yeQBUZXh0IGZpbGUgYnVzeQBFeGVjIGZvcm1hdCBlcnJvcgBJbnZhbGlkIGFyZ3VtZW50AEFyZ3VtZW50IGxpc3QgdG9vIGxvbmcAU3ltYm9saWMgbGluayBsb29wAEZpbGVuYW1lIHRvbyBsb25nAFRvbyBtYW55IG9wZW4gZmlsZXMgaW4gc3lzdGVtAE5vIGZpbGUgZGVzY3JpcHRvcnMgYXZhaWxhYmxlAEJhZCBmaWxlIGRlc2NyaXB0b3IATm8gY2hpbGQgcHJvY2VzcwBCYWQgYWRkcmVzcwBGaWxlIHRvbyBsYXJnZQBUb28gbWFueSBsaW5rcwBObyBsb2NrcyBhdmFpbGFibGUAUmVzb3VyY2UgZGVhZGxvY2sgd291bGQgb2NjdXIAU3RhdGUgbm90IHJlY292ZXJhYmxlAFByZXZpb3VzIG93bmVyIGRpZWQAT3BlcmF0aW9uIGNhbmNlbGVkAEZ1bmN0aW9uIG5vdCBpbXBsZW1lbnRlZABObyBtZXNzYWdlIG9mIGRlc2lyZWQgdHlwZQBJZGVudGlmaWVyIHJlbW92ZWQARGV2aWNlIG5vdCBhIHN0cmVhbQBObyBkYXRhIGF2YWlsYWJsZQBEZXZpY2UgdGltZW91dABPdXQgb2Ygc3RyZWFtcyByZXNvdXJjZXMATGluayBoYXMgYmVlbiBzZXZlcmVkAFByb3RvY29sIGVycm9yAEJhZCBtZXNzYWdlAEZpbGUgZGVzY3JpcHRvciBpbiBiYWQgc3RhdGUATm90IGEgc29ja2V0AERlc3RpbmF0aW9uIGFkZHJlc3MgcmVxdWlyZWQATWVzc2FnZSB0b28gbGFyZ2UAUHJvdG9jb2wgd3JvbmcgdHlwZSBmb3Igc29ja2V0AFByb3RvY29sIG5vdCBhdmFpbGFibGUAUHJvdG9jb2wgbm90IHN1cHBvcnRlZABTb2NrZXQgdHlwZSBub3Qgc3VwcG9ydGVkAE5vdCBzdXBwb3J0ZWQAUHJvdG9jb2wgZmFtaWx5IG5vdCBzdXBwb3J0ZWQAQWRkcmVzcyBmYW1pbHkgbm90IHN1cHBvcnRlZCBieSBwcm90b2NvbABBZGRyZXNzIG5vdCBhdmFpbGFibGUATmV0d29yayBpcyBkb3duAE5ldHdvcmsgdW5yZWFjaGFibGUAQ29ubmVjdGlvbiByZXNldCBieSBuZXR3b3JrAENvbm5lY3Rpb24gYWJvcnRlZABObyBidWZmZXIgc3BhY2UgYXZhaWxhYmxlAFNvY2tldCBpcyBjb25uZWN0ZWQAU29ja2V0IG5vdCBjb25uZWN0ZWQAQ2Fubm90IHNlbmQgYWZ0ZXIgc29ja2V0IHNodXRkb3duAE9wZXJhdGlvbiBhbHJlYWR5IGluIHByb2dyZXNzAE9wZXJhdGlvbiBpbiBwcm9ncmVzcwBTdGFsZSBmaWxlIGhhbmRsZQBSZW1vdGUgSS9PIGVycm9yAFF1b3RhIGV4Y2VlZGVkAE5vIG1lZGl1bSBmb3VuZABXcm9uZyBtZWRpdW0gdHlwZQBNdWx0aWhvcCBhdHRlbXB0ZWQAUmVxdWlyZWQga2V5IG5vdCBhdmFpbGFibGUAS2V5IGhhcyBleHBpcmVkAEtleSBoYXMgYmVlbiByZXZva2VkAEtleSB3YXMgcmVqZWN0ZWQgYnkgc2VydmljZQAAAAAApQJbAPABtQWMBSUBgwYdA5QE/wDHAzEDCwa8AY8BfwPKBCsA2gavAEIDTgPcAQ4EFQChBg0BlAILAjgGZAK8Av8CXQPnBAsHzwLLBe8F2wXhAh4GRQKFAIICbANvBPEA8wMYBdkA2gNMBlQCewGdA70EAABRABUCuwCzA20A/wGFBC8F+QQ4AGUBRgGfALcGqAFzAlMBAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAIQQAAAAAAAAAAC8CAAAAAAAAAAAAAAAAAAAAAAAAAAA1BEcEVgQAAAAAAAAAAAAAAAAAAAAAoAQAAAAAAAAAAAAAAAAAAAAAAABGBWAFbgVhBgAAzwEAAAAAAAAAAMkG6Qb5Bh4HOQdJB14HAAAAAAAAAAAAAAAA0XSeAFedvSqAcFIP//8+JwoAAABkAAAA6AMAABAnAACghgEAQEIPAICWmAAA4fUFGAAAADUAAABxAAAAa////877//+Sv///AAAAAAAAAAAZAAsAGRkZAAAAAAUAAAAAAAAJAAAAAAsAAAAAAAAAABkACgoZGRkDCgcAAQAJCxgAAAkGCwAACwAGGQAAABkZGQAAAAAAAAAAAAAAAAAAAAAOAAAAAAAAAAAZAAsNGRkZAA0AAAIACQ4AAAAJAA4AAA4AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAADAAAAAAAAAAAAAAAEwAAAAATAAAAAAkMAAAAAAAMAAAMAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAABAAAAAAAAAAAAAAAA8AAAAEDwAAAAAJEAAAAAAAEAAAEAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAASAAAAAAAAAAAAAAARAAAAABEAAAAACRIAAAAAABIAABIAABoAAAAaGhoAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAGgAAABoaGgAAAAAAAAkAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAABQAAAAAAAAAAAAAABcAAAAAFwAAAAAJFAAAAAAAFAAAFAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAWAAAAAAAAAAAAAAAVAAAAABUAAAAACRYAAAAAABYAABYAADAxMjM0NTY3ODlBQkNERUYAQZDGBAugBwAAAL8AAAC/AAAAPwAAAAAAAAAAAACAPwAAgD8AAAAAAAAAAAAAAAAAAAAAAAAAPwAAAL8AAAA/AAAAAAAAAAAAAIA/AAAAAAAAgD8AAAAAAACAPwAAAAAAAAA/AAAAPwAAAD8AAAAAAAAAAAAAgD8AAAAAAAAAAAAAgD8AAIA/AACAPwAAAL8AAAA/AAAAPwAAAAAAAAAAAACAPwAAgD8AAIA/AAAAAAAAAAAAAIA/AAAAvwAAAL8AAAC/AAAAAAAAAAAAAIC/AACAPwAAAAAAAIA/AAAAAAAAAAAAAAA/AAAAvwAAAL8AAAAAAAAAAAAAgL8AAAAAAACAPwAAgD8AAIA/AAAAAAAAAD8AAAA/AAAAvwAAAAAAAAAAAACAvwAAgD8AAIA/AACAPwAAgD8AAIA/AAAAvwAAAD8AAAC/AAAAAAAAAAAAAIC/AAAAPwAAAD8AAAA/AAAAAAAAgD8AAAEAAgAAAAIAAwAFAAQABwAFAAcABgAEAAAAAwAEAAMABwABAAUABgABAAYAAgADAAIABgADAAYABwAEAAUAAQAEAAEAAAAAAAAAAAAAAAAAAL8AAAAAAAAAvwAAAAAAAIA/AAAAAAAAgD8AAAAAAAAAAAAAAAAAAAAAAAAAPwAAAAAAAAC/AAAAAAAAgD8AAAAAAAAAAAAAgD8AAAAAAACAPwAAAAAAAAA/AAAAAAAAAD8AAAAAAACAPwAAAAAAAAAAAAAAAAAAgD8AAIA/AACAPwAAAL8AAAAAAAAAPwAAAAAAAIA/AAAAAAAAgD8AAIA/AAAAAAAAAAAAAIA/AAABAAIAAAACAAMAAAAAAAUAAAAAAAAAAAAAAA8AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA0AAAAMAAAAcCkBAAAAAAAAAAAAAAAAAAIAAAAAAAAAAAAAAAAAAAD//////////wAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAIAlAQAAAAAABQAAAAAAAAAAAAAAEAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAADQAAABEAAAB4KQEAAAQAAAAAAAAAAAAAAQAAAAAAAAAAAAAAAAAAAP////8KAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAGCYBAHAvAQAAlAEPdGFyZ2V0X2ZlYXR1cmVzCCsLYnVsay1tZW1vcnkrD2J1bGstbWVtb3J5LW9wdCsWY2FsbC1pbmRpcmVjdC1vdmVybG9uZysKbXVsdGl2YWx1ZSsPbXV0YWJsZS1nbG9iYWxzKxNub250cmFwcGluZy1mcHRvaW50Kw9yZWZlcmVuY2UtdHlwZXMrCHNpZ24tZXh0';

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

