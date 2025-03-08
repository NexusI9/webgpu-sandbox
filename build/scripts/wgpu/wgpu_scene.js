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
// include: /var/folders/hx/g36pxlqs1dj0kvmzszq_j3k00000gq/T/tmprebl51km.js

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
    loadPackage({"files": [{"filename": "/resources/assets/gltf/cube.gltf", "start": 0, "end": 2688}, {"filename": "/runtime/assets/shader/shader.default.wgsl", "start": 2688, "end": 4078}, {"filename": "/runtime/assets/shader/shader.grid.wgsl", "start": 4078, "end": 9260}, {"filename": "/runtime/assets/shader/shader.rotation.wgsl", "start": 9260, "end": 10725}], "remote_package_size": 10725});

  })();

// end include: /var/folders/hx/g36pxlqs1dj0kvmzszq_j3k00000gq/T/tmprebl51km.js
// include: /var/folders/hx/g36pxlqs1dj0kvmzszq_j3k00000gq/T/tmp5s7zj2tb.js

    // All the pre-js content up to here must remain later on, we need to run
    // it.
    if (Module['$ww'] || (typeof ENVIRONMENT_IS_PTHREAD != 'undefined' && ENVIRONMENT_IS_PTHREAD)) Module['preRun'] = [];
    var necessaryPreJSTasks = Module['preRun'].slice();
  // end include: /var/folders/hx/g36pxlqs1dj0kvmzszq_j3k00000gq/T/tmp5s7zj2tb.js
// include: /var/folders/hx/g36pxlqs1dj0kvmzszq_j3k00000gq/T/tmp3a6014t9.js

    if (!Module['preRun']) throw 'Module.preRun should exist because file support used it; did a pre-js delete it?';
    necessaryPreJSTasks.forEach((task) => {
      if (Module['preRun'].indexOf(task) < 0) throw 'All preRun tasks that exist before user pre-js code should remain after; did you replace Module or modify Module.preRun?';
    });
  // end include: /var/folders/hx/g36pxlqs1dj0kvmzszq_j3k00000gq/T/tmp3a6014t9.js


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
var wasmBinaryFile = 'data:application/octet-stream;base64,AGFzbQEAAAABwgIxYAJ/fwF/YAJ/fwBgA39/fwBgBX9/f39/AX9gAX8Bf2ADf39/AX9gA39+fwF+YAZ/fH9/f38Bf2AEf39/fwBgAX8AYAV/f35/fwBgBX9/f39/AGAFf39/fn4AYAZ/f39/f38AYAABf2ADf3x8AX9gA39+fwF/YAR/f39/AX9gBH9+f38Bf2AAAGAGf39/f39/AX9gB39/f39/f38Bf2ACf38BfWADf399AGADf319AGABfwF8YAF/AX5gAnx/AX9gAXwBfWACfX8Bf2ABfQF9YAF8AXxgAnx/AXxgAn9+AGAFf35+fn4AYAR/fn5/AGACfn4Bf2ADf35+AGAHf39/f39/fwBgAn9/AX5gAn9/AXxgA3x8fwF8YAN+f38Bf2ACfn8Bf2ABfAF+YAR+fn5+AX9gAn98AGACf30AYAJ+fgF8AqwONwNlbnYNX19hc3NlcnRfZmFpbAAIA2VudgRleGl0AAkDZW52KWVtc2NyaXB0ZW5fc2V0X2tleWRvd25fY2FsbGJhY2tfb25fdGhyZWFkAAMDZW52J2Vtc2NyaXB0ZW5fc2V0X2tleXVwX2NhbGxiYWNrX29uX3RocmVhZAADA2VuditlbXNjcmlwdGVuX3NldF9tb3VzZW1vdmVfY2FsbGJhY2tfb25fdGhyZWFkAAMDZW52J2Vtc2NyaXB0ZW5fc2V0X3doZWVsX2NhbGxiYWNrX29uX3RocmVhZAADA2Vudh93Z3B1RGV2aWNlQ3JlYXRlQmluZEdyb3VwTGF5b3V0AAADZW52HndncHVEZXZpY2VDcmVhdGVQaXBlbGluZUxheW91dAAAA2Vudh53Z3B1RGV2aWNlQ3JlYXRlUmVuZGVyUGlwZWxpbmUAAANlbnYkd2dwdVJlbmRlclBpcGVsaW5lR2V0QmluZEdyb3VwTGF5b3V0AAADZW52GXdncHVEZXZpY2VDcmVhdGVCaW5kR3JvdXAAAANlbnYad2dwdUJpbmRHcm91cExheW91dFJlbGVhc2UACQNlbnYZd2dwdVJlbmRlclBpcGVsaW5lUmVsZWFzZQAJA2Vudhl3Z3B1UGlwZWxpbmVMYXlvdXRSZWxlYXNlAAkDZW52F3dncHVTaGFkZXJNb2R1bGVSZWxlYXNlAAkDZW52IHdncHVSZW5kZXJQYXNzRW5jb2RlclNldFBpcGVsaW5lAAEDZW52FHdncHVRdWV1ZVdyaXRlQnVmZmVyAAoDZW52IXdncHVSZW5kZXJQYXNzRW5jb2RlclNldEJpbmRHcm91cAALA2VudiR3Z3B1UmVuZGVyUGFzc0VuY29kZXJTZXRWZXJ0ZXhCdWZmZXIADANlbnYjd2dwdVJlbmRlclBhc3NFbmNvZGVyU2V0SW5kZXhCdWZmZXIADANlbnYgd2dwdVJlbmRlclBhc3NFbmNvZGVyRHJhd0luZGV4ZWQADQNlbnYcd2dwdURldmljZUNyZWF0ZVNoYWRlck1vZHVsZQAAA2VudhZ3Z3B1RGV2aWNlQ3JlYXRlQnVmZmVyAAADZW52HGVtc2NyaXB0ZW5fd2ViZ3B1X2dldF9kZXZpY2UADgNlbnYSd2dwdURldmljZUdldFF1ZXVlAAQDZW52HmVtc2NyaXB0ZW5fcmVxdWVzdF9wb2ludGVybG9jawAAA2VudihlbXNjcmlwdGVuX3NldF9yZXNpemVfY2FsbGJhY2tfb25fdGhyZWFkAAMDZW52H2Vtc2NyaXB0ZW5fZ2V0X2VsZW1lbnRfY3NzX3NpemUABQNlbnYfZW1zY3JpcHRlbl9zZXRfZWxlbWVudF9jc3Nfc2l6ZQAPA2VudhR3Z3B1U3dhcENoYWluUmVsZWFzZQAJA2VudhB3Z3B1UXVldWVSZWxlYXNlAAkDZW52EXdncHVEZXZpY2VSZWxlYXNlAAkDZW52IndncHVTd2FwQ2hhaW5HZXRDdXJyZW50VGV4dHVyZVZpZXcABANlbnYed2dwdURldmljZUNyZWF0ZUNvbW1hbmRFbmNvZGVyAAADZW52IXdncHVDb21tYW5kRW5jb2RlckJlZ2luUmVuZGVyUGFzcwAAA2Vudhh3Z3B1UmVuZGVyUGFzc0VuY29kZXJFbmQACQNlbnYYd2dwdUNvbW1hbmRFbmNvZGVyRmluaXNoAAADZW52D3dncHVRdWV1ZVN1Ym1pdAACA2Vudhx3Z3B1UmVuZGVyUGFzc0VuY29kZXJSZWxlYXNlAAkDZW52GXdncHVDb21tYW5kRW5jb2RlclJlbGVhc2UACQNlbnYYd2dwdUNvbW1hbmRCdWZmZXJSZWxlYXNlAAkDZW52FndncHVUZXh0dXJlVmlld1JlbGVhc2UACQNlbnYYZW1zY3Jp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CnEhggogggpFDQEgAygCfCGDCiCDCigC4AEhhAogAygCfCGFCiCFCigC5AEhhgogAygCfCGHCiCHCigCjAEhiAogAygCJCGJCkEFIYoKIIkKIIoKdCGLCiCICiCLCmohjAogjAooAgAhjQoghgogjQoghAoRgYCAgACAgICAACADKAJ8IY4KII4KKALgASGPCiADKAJ8IZAKIJAKKALkASGRCiADKAJ8IZIKIJIKKAKMASGTCiADKAIkIZQKQQUhlQoglAoglQp0IZYKIJMKIJYKaiGXCiCXCigCBCGYCiCRCiCYCiCPChGBgICAAICAgIAAIAMoAnwhmQogAygCfCGaCiCaCigCjAEhmwogAygCJCGcCkEFIZ0KIJwKIJ0KdCGeCiCbCiCeCmohnwognwooAhwhoAogAygCfCGhCiChCigCjAEhogogAygCJCGjCkEFIaQKIKMKIKQKdCGlCiCiCiClCmohpgogpgooAhghpwogmQogoAogpwoQz4CAgAAgAygCfCGoCiADKAJ8IakKIKkKKAKMASGqCiADKAIkIasKQQUhrAogqwogrAp0Ia0KIKoKIK0KaiGuCkEMIa8KIK4KIK8KaiGwCiCoCiCwChDQgICAACADKAIkIbEKQQEhsgogsQogsgpqIbMKIAMgswo2AiQMAAsLIAMoAnwhtAogtAooAuABIbUKIAMoAnwhtgogtgooAuQBIbcKIAMoAnwhuAoguAooAowBIbkKILcKILkKILUKEYGAgIAAgICAgABBACG6CiADILoKNgIgAkADQCADKAIgIbsKIAMoAnwhvAogvAooApwBIb0KILsKIL0KSSG+CkEBIb8KIL4KIL8KcSHACiDACkUNASADKAJ8IcEKIMEKKALgASHCCiADKAJ8IcMKIMMKKALkASHECiADKAJ8IcUKIMUKKAKYASHGCiADKAIgIccKQSghyAogxwogyApsIckKIMYKIMkKaiHKCiDKCigCACHLCiDECiDLCiDCChGBgICAAICAgIAAQQAhzAogAyDMCjYCHAJAA0AgAygCHCHNCiADKAJ8Ic4KIM4KKAKYASHPCiADKAIgIdAKQSgh0Qog0Aog0QpsIdIKIM8KINIKaiHTCiDTCigCCCHUCiDNCiDUCkkh1QpBASHWCiDVCiDWCnEh1wog1wpFDQEgAygCfCHYCiADKAJ8IdkKINkKKAKYASHaCiADKAIgIdsKQSgh3Aog2wog3ApsId0KINoKIN0KaiHeCiDeCigCBCHfCiADKAIcIeAKQQUh4Qog4Aog4Qp0IeIKIN8KIOIKaiHjCiDjCigCHCHkCiADKAJ8IeUKIOUKKAKYASHmCiADKAIgIecKQSgh6Aog5wog6ApsIekKIOYKIOkKaiHqCiDqCigCBCHrCiADKAIcIewKQQUh7Qog7Aog7Qp0Ie4KIOsKIO4KaiHvCiDvCigCGCHwCiDYCiDkCiDwChDPgICAACADKAJ8IfEKIAMoAnwh8gog8gooApgBIfMKIAMoAiAh9ApBKCH1CiD0CiD1Cmwh9gog8wog9gpqIfcKIPcKKAIEIfgKIAMoAhwh+QpBBSH6CiD5CiD6CnQh+wog+Aog+wpqIfwKQQwh/Qog/Aog/QpqIf4KIPEKIP4KENCAgIAAIAMoAhwh/wpBASGACyD/CiCAC2ohgQsgAyCBCzYCHAwACwsgAygCfCGCCyCCCygC4AEhgwsgAygCfCGECyCECygC5AEhhQsgAygCfCGGCyCGCygCmAEhhwsgAygCICGIC0EoIYkLIIgLIIkLbCGKCyCHCyCKC2ohiwsgiwsoAgQhjAsghQsgjAsggwsRgYCAgACAgICAAEEAIY0LIAMgjQs2AhgCQANAIAMoAhghjgsgAygCfCGPCyCPCygCmAEhkAsgAygCICGRC0EoIZILIJELIJILbCGTCyCQCyCTC2ohlAsglAsoAhAhlQsgjgsglQtJIZYLQQEhlwsglgsglwtxIZgLIJgLRQ0BIAMoAnwhmQsgAygCfCGaCyCaCygCmAEhmwsgAygCICGcC0EoIZ0LIJwLIJ0LbCGeCyCbCyCeC2ohnwsgnwsoAgwhoAsgAygCGCGhC0EFIaILIKELIKILdCGjCyCgCyCjC2ohpAsgpAsoAhwhpQsgAygCfCGmCyCmCygCmAEhpwsgAygCICGoC0EoIakLIKgLIKkLbCGqCyCnCyCqC2ohqwsgqwsoAgwhrAsgAygCGCGtC0EFIa4LIK0LIK4LdCGvCyCsCyCvC2ohsAsgsAsoAhghsQsgmQsgpQsgsQsQz4CAgAAgAygCfCGyCyADKAJ8IbMLILMLKAKYASG0CyADKAIgIbULQSghtgsgtQsgtgtsIbcLILQLILcLaiG4CyC4CygCDCG5CyADKAIYIboLQQUhuwsgugsguwt0IbwLILkLILwLaiG9C0EMIb4LIL0LIL4LaiG/CyCyCyC/CxDQgICAACADKAIYIcALQQEhwQsgwAsgwQtqIcILIAMgwgs2AhgMAAsLIAMoAnwhwwsgwwsoAuABIcQLIAMoAnwhxQsgxQsoAuQBIcYLIAMoAnwhxwsgxwsoApgBIcgLIAMoAiAhyQtBKCHKCyDJCyDKC2whywsgyAsgywtqIcwLIMwLKAIMIc0LIMYLIM0LIMQLEYGAgIAAgICAgAAgAygCfCHOCyADKAJ8Ic8LIM8LKAKYASHQCyADKAIgIdELQSgh0gsg0Qsg0gtsIdMLINALINMLaiHUCyDUCygCJCHVCyADKAJ8IdYLINYLKAKYASHXCyADKAIgIdgLQSgh2Qsg2Asg2QtsIdoLINcLINoLaiHbCyDbCygCICHcCyDOCyDVCyDcCxDPgICAACADKAJ8Id0LIAMoAnwh3gsg3gsoApgBId8LIAMoAiAh4AtBKCHhCyDgCyDhC2wh4gsg3wsg4gtqIeMLQRQh5Asg4wsg5AtqIeULIN0LIOULENCAgIAAIAMoAiAh5gtBASHnCyDmCyDnC2oh6AsgAyDoCzYCIAwACwsgAygCfCHpCyDpCygC4AEh6gsgAygCfCHrCyDrCygC5AEh7AsgAygCfCHtCyDtCygCmAEh7gsg7Asg7gsg6gsRgYCAgACAgICAAEEAIe8LIAMg7ws2AhQCQANAIAMoAhQh8AsgAygCfCHxCyDxCygCpAEh8gsg8Asg8gtJIfMLQQEh9Asg8wsg9AtxIfULIPULRQ0BIAMoAnwh9gsg9gsoAuABIfcLIAMoAnwh+Asg+AsoAuQBIfkLIAMoAnwh+gsg+gsoAqABIfsLIAMoAhQh/AtBBCH9CyD8CyD9C3Qh/gsg+wsg/gtqIf8LIP8LKAIAIYAMIPkLIIAMIPcLEYGAgIAAgICAgAAgAygCfCGBDCADKAJ8IYIMIIIMKAKgASGDDCADKAIUIYQMQQQhhQwghAwghQx0IYYMIIMMIIYMaiGHDEEEIYgMIIcMIIgMaiGJDCCBDCCJDBDQgICAACADKAIUIYoMQQEhiwwgigwgiwxqIYwMIAMgjAw2AhQMAAsLIAMoAnwhjQwgjQwoAuABIY4MIAMoAnwhjwwgjwwoAuQBIZAMIAMoAnwhkQwgkQwoAqABIZIMIJAMIJIMII4MEYGAgIAAgICAgAAgAygCfCGTDCADKAJ8IZQMIJQMKAK4ASGVDCADKAJ8IZYMIJYMKAK0ASGXDCCTDCCVDCCXDBDPgICAACADKAJ8IZgMIAMoAnwhmQxBqAEhmgwgmQwgmgxqIZsMIJgMIJsMENCAgIAAQQAhnAwgAyCcDDYCEAJAA0AgAygCECGdDCADKAJ8IZ4MIJ4MKALAASGfDCCdDCCfDEkhoAxBASGhDCCgDCChDHEhogwgogxFDQEgAygCfCGjDCCjDCgC4AEhpAwgAygCfCGlDCClDCgC5AEhpgwgAygCfCGnDCCnDCgCvAEhqAwgAygCECGpDEECIaoMIKkMIKoMdCGrDCCoDCCrDGohrAwgrAwoAgAhrQwgpgwgrQwgpAwRgYCAgACAgICAACADKAIQIa4MQQEhrwwgrgwgrwxqIbAMIAMgsAw2AhAMAAsLIAMoAnwhsQwgsQwoAuABIbIMIAMoAnwhswwgswwoAuQBIbQMIAMoAnwhtQwgtQwoArwBIbYMILQMILYMILIMEYGAgIAAgICAgABBACG3DCADILcMNgIMAkADQCADKAIMIbgMIAMoAnwhuQwguQwoAsgBIboMILgMILoMSSG7DEEBIbwMILsMILwMcSG9DCC9DEUNASADKAJ8Ib4MIL4MKALgASG/DCADKAJ8IcAMIMAMKALkASHBDCADKAJ8IcIMIMIMKALEASHDDCADKAIMIcQMQQIhxQwgxAwgxQx0IcYMIMMMIMYMaiHHDCDHDCgCACHIDCDBDCDIDCC/DBGBgICAAICAgIAAIAMoAgwhyQxBASHKDCDJDCDKDGohywwgAyDLDDYCDAwACwsgAygCfCHMDCDMDCgC4AEhzQwgAygCfCHODCDODCgC5AEhzwwgAygCfCHQDCDQDCgCxAEh0Qwgzwwg0QwgzQwRgYCAgACAgICAACADKAJ4IdIMIAMoAnwh0wxB3AEh1Awg0wwg1AxqIdUMIAMoAnwh1gxB6AEh1wwg1gwg1wxqIdgMIAMoAnwh2Qwg2QwoAgQh2gwg1Qwg2Awg2gwg0gwRgoCAgACAgICAACADKAJ8IdsMINsMKALgASHcDCADKAJ8Id0MIN0MKALkASHeDCADKAJ8Id8MIN4MIN8MINwMEYGAgIAAgICAgAALQYABIeAMIAMg4AxqIeEMIOEMJICAgIAADwvE4gEB6xh/I4CAgIAAIQFB4AAhAiABIAJrIQMgAySAgICAACADIAA2AlhBACEEIAMgBDYCVAJAAkADQCADKAJUIQUgAygCWCEGIAYoAjAhByAFIAdJIQhBASEJIAggCXEhCiAKRQ0BQQAhCyADIAs2AlACQANAIAMoAlAhDCADKAJYIQ0gDSgCLCEOIAMoAlQhD0EwIRAgDyAQbCERIA4gEWohEiASKAIIIRMgDCATSSEUQQEhFSAUIBVxIRYgFkUNASADKAJYIRcgFygCLCEYIAMoAlQhGUEwIRogGSAabCEbIBggG2ohHCAcKAIEIR0gAygCUCEeQcgAIR8gHiAfbCEgIB0gIGohISAhKAIEISJBACEjICIgI0chJEEBISUgJCAlcSEmAkAgJkUNACADKAJYIScgJygCLCEoIAMoAlQhKUEwISogKSAqbCErICggK2ohLCAsKAIEIS0gAygCUCEuQcgAIS8gLiAvbCEwIC0gMGohMSAxKAIEITIgAygCWCEzIDMoAkAhNCAyIDRLITVBASE2IDUgNnEhNwJAIDdFDQBBfyE4IAMgODYCXAwGCyADKAJYITkgOSgCPCE6IAMoAlghOyA7KAIsITwgAygCVCE9QTAhPiA9ID5sIT8gPCA/aiFAIEAoAgQhQSADKAJQIUJByAAhQyBCIENsIUQgQSBEaiFFIEUoAgQhRkEBIUcgRiBHayFIQdgBIUkgSCBJbCFKIDogSmohSyADKAJYIUwgTCgCLCFNIAMoAlQhTkEwIU8gTiBPbCFQIE0gUGohUSBRKAIEIVIgAygCUCFTQcgAIVQgUyBUbCFVIFIgVWohViBWIEs2AgQLIAMoAlghVyBXKAIsIVggAygCVCFZQTAhWiBZIFpsIVsgWCBbaiFcIFwoAgQhXSADKAJQIV5ByAAhXyBeIF9sIWAgXSBgaiFhIGEoAgghYkEAIWMgYiBjRyFkQQEhZSBkIGVxIWYCQCBmRQ0AIAMoAlghZyBnKAIsIWggAygCVCFpQTAhaiBpIGpsIWsgaCBraiFsIGwoAgQhbSADKAJQIW5ByAAhbyBuIG9sIXAgbSBwaiFxIHEoAgghciADKAJYIXMgcygCOCF0IHIgdEshdUEBIXYgdSB2cSF3AkAgd0UNAEF/IXggAyB4NgJcDAYLIAMoAlgheSB5KAI0IXogAygCWCF7IHsoAiwhfCADKAJUIX1BMCF+IH0gfmwhfyB8IH9qIYABIIABKAIEIYEBIAMoAlAhggFByAAhgwEgggEggwFsIYQBIIEBIIQBaiGFASCFASgCCCGGAUEBIYcBIIYBIIcBayGIAUGwCSGJASCIASCJAWwhigEgeiCKAWohiwEgAygCWCGMASCMASgCLCGNASADKAJUIY4BQTAhjwEgjgEgjwFsIZABII0BIJABaiGRASCRASgCBCGSASADKAJQIZMBQcgAIZQBIJMBIJQBbCGVASCSASCVAWohlgEglgEgiwE2AggLQQAhlwEgAyCXATYCTAJAA0AgAygCTCGYASADKAJYIZkBIJkBKAIsIZoBIAMoAlQhmwFBMCGcASCbASCcAWwhnQEgmgEgnQFqIZ4BIJ4BKAIEIZ8BIAMoAlAhoAFByAAhoQEgoAEgoQFsIaIBIJ8BIKIBaiGjASCjASgCECGkASCYASCkAUkhpQFBASGmASClASCmAXEhpwEgpwFFDQEgAygCWCGoASCoASgCLCGpASADKAJUIaoBQTAhqwEgqgEgqwFsIawBIKkBIKwBaiGtASCtASgCBCGuASADKAJQIa8BQcgAIbABIK8BILABbCGxASCuASCxAWohsgEgsgEoAgwhswEgAygCTCG0AUEEIbUBILQBILUBdCG2ASCzASC2AWohtwEgtwEoAgwhuAFBACG5ASC4ASC5AUchugFBASG7ASC6ASC7AXEhvAECQAJAILwBRQ0AIAMoAlghvQEgvQEoAiwhvgEgAygCVCG/AUEwIcABIL8BIMABbCHBASC+ASDBAWohwgEgwgEoAgQhwwEgAygCUCHEAUHIACHFASDEASDFAWwhxgEgwwEgxgFqIccBIMcBKAIMIcgBIAMoAkwhyQFBBCHKASDJASDKAXQhywEgyAEgywFqIcwBIMwBKAIMIc0BIAMoAlghzgEgzgEoAkAhzwEgzQEgzwFLIdABQQEh0QEg0AEg0QFxIdIBINIBRQ0BC0F/IdMBIAMg0wE2AlwMBwsgAygCWCHUASDUASgCPCHVASADKAJYIdYBINYBKAIsIdcBIAMoAlQh2AFBMCHZASDYASDZAWwh2gEg1wEg2gFqIdsBINsBKAIEIdwBIAMoAlAh3QFByAAh3gEg3QEg3gFsId8BINwBIN8BaiHgASDgASgCDCHhASADKAJMIeIBQQQh4wEg4gEg4wF0IeQBIOEBIOQBaiHlASDlASgCDCHmAUEBIecBIOYBIOcBayHoAUHYASHpASDoASDpAWwh6gEg1QEg6gFqIesBIAMoAlgh7AEg7AEoAiwh7QEgAygCVCHuAUEwIe8BIO4BIO8BbCHwASDtASDwAWoh8QEg8QEoAgQh8gEgAygCUCHzAUHIACH0ASDzASD0AWwh9QEg8gEg9QFqIfYBIPYBKAIMIfcBIAMoAkwh+AFBBCH5ASD4ASD5AXQh+gEg9wEg+gFqIfsBIPsBIOsBNgIMIAMoAkwh/AFBASH9ASD8ASD9AWoh/gEgAyD+ATYCTAwACwtBACH/ASADIP8BNgJIAkADQCADKAJIIYACIAMoAlghgQIggQIoAiwhggIgAygCVCGDAkEwIYQCIIMCIIQCbCGFAiCCAiCFAmohhgIghgIoAgQhhwIgAygCUCGIAkHIACGJAiCIAiCJAmwhigIghwIgigJqIYsCIIsCKAIYIYwCIIACIIwCSSGNAkEBIY4CII0CII4CcSGPAiCPAkUNAUEAIZACIAMgkAI2AkQCQANAIAMoAkQhkQIgAygCWCGSAiCSAigCLCGTAiADKAJUIZQCQTAhlQIglAIglQJsIZYCIJMCIJYCaiGXAiCXAigCBCGYAiADKAJQIZkCQcgAIZoCIJkCIJoCbCGbAiCYAiCbAmohnAIgnAIoAhQhnQIgAygCSCGeAkEDIZ8CIJ4CIJ8CdCGgAiCdAiCgAmohoQIgoQIoAgQhogIgkQIgogJJIaMCQQEhpAIgowIgpAJxIaUCIKUCRQ0BIAMoAlghpgIgpgIoAiwhpwIgAygCVCGoAkEwIakCIKgCIKkCbCGqAiCnAiCqAmohqwIgqwIoAgQhrAIgAygCUCGtAkHIACGuAiCtAiCuAmwhrwIgrAIgrwJqIbACILACKAIUIbECIAMoAkghsgJBAyGzAiCyAiCzAnQhtAIgsQIgtAJqIbUCILUCKAIAIbYCIAMoAkQhtwJBBCG4AiC3AiC4AnQhuQIgtgIguQJqIboCILoCKAIMIbsCQQAhvAIguwIgvAJHIb0CQQEhvgIgvQIgvgJxIb8CAkACQCC/AkUNACADKAJYIcACIMACKAIsIcECIAMoAlQhwgJBMCHDAiDCAiDDAmwhxAIgwQIgxAJqIcUCIMUCKAIEIcYCIAMoAlAhxwJByAAhyAIgxwIgyAJsIckCIMYCIMkCaiHKAiDKAigCFCHLAiADKAJIIcwCQQMhzQIgzAIgzQJ0Ic4CIMsCIM4CaiHPAiDPAigCACHQAiADKAJEIdECQQQh0gIg0QIg0gJ0IdMCINACINMCaiHUAiDUAigCDCHVAiADKAJYIdYCINYCKAJAIdcCINUCINcCSyHYAkEBIdkCINgCINkCcSHaAiDaAkUNAQtBfyHbAiADINsCNgJcDAkLIAMoAlgh3AIg3AIoAjwh3QIgAygCWCHeAiDeAigCLCHfAiADKAJUIeACQTAh4QIg4AIg4QJsIeICIN8CIOICaiHjAiDjAigCBCHkAiADKAJQIeUCQcgAIeYCIOUCIOYCbCHnAiDkAiDnAmoh6AIg6AIoAhQh6QIgAygCSCHqAkEDIesCIOoCIOsCdCHsAiDpAiDsAmoh7QIg7QIoAgAh7gIgAygCRCHvAkEEIfACIO8CIPACdCHxAiDuAiDxAmoh8gIg8gIoAgwh8wJBASH0AiDzAiD0Amsh9QJB2AEh9gIg9QIg9gJsIfcCIN0CIPcCaiH4AiADKAJYIfkCIPkCKAIsIfoCIAMoAlQh+wJBMCH8AiD7AiD8Amwh/QIg+gIg/QJqIf4CIP4CKAIEIf8CIAMoAlAhgANByAAhgQMggAMggQNsIYIDIP8CIIIDaiGDAyCDAygCFCGEAyADKAJIIYUDQQMhhgMghQMghgN0IYcDIIQDIIcDaiGIAyCIAygCACGJAyADKAJEIYoDQQQhiwMgigMgiwN0IYwDIIkDIIwDaiGNAyCNAyD4AjYCDCADKAJEIY4DQQEhjwMgjgMgjwNqIZADIAMgkAM2AkQMAAsLIAMoAkghkQNBASGSAyCRAyCSA2ohkwMgAyCTAzYCSAwACwsgAygCWCGUAyCUAygCLCGVAyADKAJUIZYDQTAhlwMglgMglwNsIZgDIJUDIJgDaiGZAyCZAygCBCGaAyADKAJQIZsDQcgAIZwDIJsDIJwDbCGdAyCaAyCdA2ohngMgngMoAighnwMCQCCfA0UNACADKAJYIaADIKADKAIsIaEDIAMoAlQhogNBMCGjAyCiAyCjA2whpAMgoQMgpANqIaUDIKUDKAIEIaYDIAMoAlAhpwNByAAhqAMgpwMgqANsIakDIKYDIKkDaiGqAyCqAygCLCGrA0EAIawDIKsDIKwDRyGtA0EBIa4DIK0DIK4DcSGvAwJAAkAgrwNFDQAgAygCWCGwAyCwAygCLCGxAyADKAJUIbIDQTAhswMgsgMgswNsIbQDILEDILQDaiG1AyC1AygCBCG2AyADKAJQIbcDQcgAIbgDILcDILgDbCG5AyC2AyC5A2ohugMgugMoAiwhuwMgAygCWCG8AyC8AygCSCG9AyC7AyC9A0shvgNBASG/AyC+AyC/A3EhwAMgwANFDQELQX8hwQMgAyDBAzYCXAwGCyADKAJYIcIDIMIDKAJEIcMDIAMoAlghxAMgxAMoAiwhxQMgAygCVCHGA0EwIccDIMYDIMcDbCHIAyDFAyDIA2ohyQMgyQMoAgQhygMgAygCUCHLA0HIACHMAyDLAyDMA2whzQMgygMgzQNqIc4DIM4DKAIsIc8DQQEh0AMgzwMg0ANrIdEDQdAAIdIDINEDINIDbCHTAyDDAyDTA2oh1AMgAygCWCHVAyDVAygCLCHWAyADKAJUIdcDQTAh2AMg1wMg2ANsIdkDINYDINkDaiHaAyDaAygCBCHbAyADKAJQIdwDQcgAId0DINwDIN0DbCHeAyDbAyDeA2oh3wMg3wMg1AM2AixBACHgAyADIOADNgJAAkADQCADKAJAIeEDIAMoAlgh4gMg4gMoAiwh4wMgAygCVCHkA0EwIeUDIOQDIOUDbCHmAyDjAyDmA2oh5wMg5wMoAgQh6AMgAygCUCHpA0HIACHqAyDpAyDqA2wh6wMg6AMg6wNqIewDIOwDKAI0Ie0DIOEDIO0DSSHuA0EBIe8DIO4DIO8DcSHwAyDwA0UNASADKAJYIfEDIPEDKAIsIfIDIAMoAlQh8wNBMCH0AyDzAyD0A2wh9QMg8gMg9QNqIfYDIPYDKAIEIfcDIAMoAlAh+ANByAAh+QMg+AMg+QNsIfoDIPcDIPoDaiH7AyD7AygCMCH8AyADKAJAIf0DQQQh/gMg/QMg/gN0If8DIPwDIP8DaiGABCCABCgCDCGBBEEAIYIEIIEEIIIERyGDBEEBIYQEIIMEIIQEcSGFBAJAAkAghQRFDQAgAygCWCGGBCCGBCgCLCGHBCADKAJUIYgEQTAhiQQgiAQgiQRsIYoEIIcEIIoEaiGLBCCLBCgCBCGMBCADKAJQIY0EQcgAIY4EII0EII4EbCGPBCCMBCCPBGohkAQgkAQoAjAhkQQgAygCQCGSBEEEIZMEIJIEIJMEdCGUBCCRBCCUBGohlQQglQQoAgwhlgQgAygCWCGXBCCXBCgCQCGYBCCWBCCYBEshmQRBASGaBCCZBCCaBHEhmwQgmwRFDQELQX8hnAQgAyCcBDYCXAwICyADKAJYIZ0EIJ0EKAI8IZ4EIAMoAlghnwQgnwQoAiwhoAQgAygCVCGhBEEwIaIEIKEEIKIEbCGjBCCgBCCjBGohpAQgpAQoAgQhpQQgAygCUCGmBEHIACGnBCCmBCCnBGwhqAQgpQQgqARqIakEIKkEKAIwIaoEIAMoAkAhqwRBBCGsBCCrBCCsBHQhrQQgqgQgrQRqIa4EIK4EKAIMIa8EQQEhsAQgrwQgsARrIbEEQdgBIbIEILEEILIEbCGzBCCeBCCzBGohtAQgAygCWCG1BCC1BCgCLCG2BCADKAJUIbcEQTAhuAQgtwQguARsIbkEILYEILkEaiG6BCC6BCgCBCG7BCADKAJQIbwEQcgAIb0EILwEIL0EbCG+BCC7BCC+BGohvwQgvwQoAjAhwAQgAygCQCHBBEEEIcIEIMEEIMIEdCHDBCDABCDDBGohxAQgxAQgtAQ2AgwgAygCQCHFBEEBIcYEIMUEIMYEaiHHBCADIMcENgJADAALCwtBACHIBCADIMgENgI8AkADQCADKAI8IckEIAMoAlghygQgygQoAiwhywQgAygCVCHMBEEwIc0EIMwEIM0EbCHOBCDLBCDOBGohzwQgzwQoAgQh0AQgAygCUCHRBEHIACHSBCDRBCDSBGwh0wQg0AQg0wRqIdQEINQEKAI8IdUEIMkEINUESSHWBEEBIdcEINYEINcEcSHYBCDYBEUNASADKAJYIdkEINkEKAIsIdoEIAMoAlQh2wRBMCHcBCDbBCDcBGwh3QQg2gQg3QRqId4EIN4EKAIEId8EIAMoAlAh4ARByAAh4QQg4AQg4QRsIeIEIN8EIOIEaiHjBCDjBCgCOCHkBCADKAI8IeUEQRQh5gQg5QQg5gRsIecEIOQEIOcEaiHoBCDoBCgCBCHpBEEAIeoEIOkEIOoERyHrBEEBIewEIOsEIOwEcSHtBAJAAkAg7QRFDQAgAygCWCHuBCDuBCgCLCHvBCADKAJUIfAEQTAh8QQg8AQg8QRsIfIEIO8EIPIEaiHzBCDzBCgCBCH0BCADKAJQIfUEQcgAIfYEIPUEIPYEbCH3BCD0BCD3BGoh+AQg+AQoAjgh+QQgAygCPCH6BEEUIfsEIPoEIPsEbCH8BCD5BCD8BGoh/QQg/QQoAgQh/gQgAygCWCH/BCD/BCgCOCGABSD+BCCABUshgQVBASGCBSCBBSCCBXEhgwUggwVFDQELQX8hhAUgAyCEBTYCXAwHCyADKAJYIYUFIIUFKAI0IYYFIAMoAlghhwUghwUoAiwhiAUgAygCVCGJBUEwIYoFIIkFIIoFbCGLBSCIBSCLBWohjAUgjAUoAgQhjQUgAygCUCGOBUHIACGPBSCOBSCPBWwhkAUgjQUgkAVqIZEFIJEFKAI4IZIFIAMoAjwhkwVBFCGUBSCTBSCUBWwhlQUgkgUglQVqIZYFIJYFKAIEIZcFQQEhmAUglwUgmAVrIZkFQbAJIZoFIJkFIJoFbCGbBSCGBSCbBWohnAUgAygCWCGdBSCdBSgCLCGeBSADKAJUIZ8FQTAhoAUgnwUgoAVsIaEFIJ4FIKEFaiGiBSCiBSgCBCGjBSADKAJQIaQFQcgAIaUFIKQFIKUFbCGmBSCjBSCmBWohpwUgpwUoAjghqAUgAygCPCGpBUEUIaoFIKkFIKoFbCGrBSCoBSCrBWohrAUgrAUgnAU2AgQgAygCPCGtBUEBIa4FIK0FIK4FaiGvBSADIK8FNgI8DAALCyADKAJQIbAFQQEhsQUgsAUgsQVqIbIFIAMgsgU2AlAMAAsLIAMoAlQhswVBASG0BSCzBSC0BWohtQUgAyC1BTYCVAwACwtBACG2BSADILYFNgI4AkADQCADKAI4IbcFIAMoAlghuAUguAUoAkAhuQUgtwUguQVJIboFQQEhuwUgugUguwVxIbwFILwFRQ0BIAMoAlghvQUgvQUoAjwhvgUgAygCOCG/BUHYASHABSC/BSDABWwhwQUgvgUgwQVqIcIFIMIFKAIcIcMFQQAhxAUgwwUgxAVHIcUFQQEhxgUgxQUgxgVxIccFAkAgxwVFDQAgAygCWCHIBSDIBSgCPCHJBSADKAI4IcoFQdgBIcsFIMoFIMsFbCHMBSDJBSDMBWohzQUgzQUoAhwhzgUgAygCWCHPBSDPBSgCSCHQBSDOBSDQBUsh0QVBASHSBSDRBSDSBXEh0wUCQCDTBUUNAEF/IdQFIAMg1AU2AlwMBAsgAygCWCHVBSDVBSgCRCHWBSADKAJYIdcFINcFKAI8IdgFIAMoAjgh2QVB2AEh2gUg2QUg2gVsIdsFINgFINsFaiHcBSDcBSgCHCHdBUEBId4FIN0FIN4FayHfBUHQACHgBSDfBSDgBWwh4QUg1gUg4QVqIeIFIAMoAlgh4wUg4wUoAjwh5AUgAygCOCHlBUHYASHmBSDlBSDmBWwh5wUg5AUg5wVqIegFIOgFIOIFNgIcCyADKAJYIekFIOkFKAI8IeoFIAMoAjgh6wVB2AEh7AUg6wUg7AVsIe0FIOoFIO0FaiHuBSDuBSgCqAEh7wUCQCDvBUUNACADKAJYIfAFIPAFKAI8IfEFIAMoAjgh8gVB2AEh8wUg8gUg8wVsIfQFIPEFIPQFaiH1BSD1BSgCsAEh9gVBACH3BSD2BSD3BUch+AVBASH5BSD4BSD5BXEh+gUCQAJAIPoFRQ0AIAMoAlgh+wUg+wUoAjwh/AUgAygCOCH9BUHYASH+BSD9BSD+BWwh/wUg/AUg/wVqIYAGIIAGKAKwASGBBiADKAJYIYIGIIIGKAJIIYMGIIEGIIMGSyGEBkEBIYUGIIQGIIUGcSGGBiCGBkUNAQtBfyGHBiADIIcGNgJcDAQLIAMoAlghiAYgiAYoAkQhiQYgAygCWCGKBiCKBigCPCGLBiADKAI4IYwGQdgBIY0GIIwGII0GbCGOBiCLBiCOBmohjwYgjwYoArABIZAGQQEhkQYgkAYgkQZrIZIGQdAAIZMGIJIGIJMGbCGUBiCJBiCUBmohlQYgAygCWCGWBiCWBigCPCGXBiADKAI4IZgGQdgBIZkGIJgGIJkGbCGaBiCXBiCaBmohmwYgmwYglQY2ArABIAMoAlghnAYgnAYoAjwhnQYgAygCOCGeBkHYASGfBiCeBiCfBmwhoAYgnQYgoAZqIaEGIKEGKAK8ASGiBkEAIaMGIKIGIKMGRyGkBkEBIaUGIKQGIKUGcSGmBgJAAkAgpgZFDQAgAygCWCGnBiCnBigCPCGoBiADKAI4IakGQdgBIaoGIKkGIKoGbCGrBiCoBiCrBmohrAYgrAYoArwBIa0GIAMoAlghrgYgrgYoAkghrwYgrQYgrwZLIbAGQQEhsQYgsAYgsQZxIbIGILIGRQ0BC0F/IbMGIAMgswY2AlwMBAsgAygCWCG0BiC0BigCRCG1BiADKAJYIbYGILYGKAI8IbcGIAMoAjghuAZB2AEhuQYguAYguQZsIboGILcGILoGaiG7BiC7BigCvAEhvAZBASG9BiC8BiC9BmshvgZB0AAhvwYgvgYgvwZsIcAGILUGIMAGaiHBBiADKAJYIcIGIMIGKAI8IcMGIAMoAjghxAZB2AEhxQYgxAYgxQZsIcYGIMMGIMYGaiHHBiDHBiDBBjYCvAELIAMoAlghyAYgyAYoAjwhyQYgAygCOCHKBkHYASHLBiDKBiDLBmwhzAYgyQYgzAZqIc0GIM0GKAIcIc4GQQAhzwYgzgYgzwZHIdAGQQEh0QYg0AYg0QZxIdIGAkAg0gZFDQAgAygCWCHTBiDTBigCPCHUBiADKAI4IdUGQdgBIdYGINUGINYGbCHXBiDUBiDXBmoh2AYg2AYoAhwh2QYg2QYoAhAh2gYgAygCWCHbBiDbBigCPCHcBiADKAI4Id0GQdgBId4GIN0GIN4GbCHfBiDcBiDfBmoh4AYg4AYg2gY2AhgLIAMoAlgh4QYg4QYoAjwh4gYgAygCOCHjBkHYASHkBiDjBiDkBmwh5QYg4gYg5QZqIeYGIOYGKAIYIecGAkAg5wYNACADKAJYIegGIOgGKAI8IekGIAMoAjgh6gZB2AEh6wYg6gYg6wZsIewGIOkGIOwGaiHtBiDtBigCDCHuBiADKAJYIe8GIO8GKAI8IfAGIAMoAjgh8QZB2AEh8gYg8QYg8gZsIfMGIPAGIPMGaiH0BiD0BigCBCH1BiDuBiD1BhDMgICAACH2BiADKAJYIfcGIPcGKAI8IfgGIAMoAjgh+QZB2AEh+gYg+QYg+gZsIfsGIPgGIPsGaiH8BiD8BiD2BjYCGAsgAygCOCH9BkEBIf4GIP0GIP4GaiH/BiADIP8GNgI4DAALC0EAIYAHIAMggAc2AjQCQANAIAMoAjQhgQcgAygCWCGCByCCBygCYCGDByCBByCDB0khhAdBASGFByCEByCFB3EhhgcghgdFDQEgAygCWCGHByCHBygCXCGIByADKAI0IYkHQTAhigcgiQcgigdsIYsHIIgHIIsHaiGMByCMBygCBCGNB0EAIY4HII0HII4HRyGPB0EBIZAHII8HIJAHcSGRBwJAIJEHRQ0AIAMoAlghkgcgkgcoAlwhkwcgAygCNCGUB0EwIZUHIJQHIJUHbCGWByCTByCWB2ohlwcglwcoAgQhmAcgAygCWCGZByCZBygCWCGaByCYByCaB0shmwdBASGcByCbByCcB3EhnQcCQCCdB0UNAEF/IZ4HIAMgngc2AlwMBAsgAygCWCGfByCfBygCVCGgByADKAJYIaEHIKEHKAJcIaIHIAMoAjQhowdBMCGkByCjByCkB2whpQcgogcgpQdqIaYHIKYHKAIEIacHQQEhqAcgpwcgqAdrIakHQSQhqgcgqQcgqgdsIasHIKAHIKsHaiGsByADKAJYIa0HIK0HKAJcIa4HIAMoAjQhrwdBMCGwByCvByCwB2whsQcgrgcgsQdqIbIHILIHIKwHNgIECyADKAJYIbMHILMHKAJcIbQHIAMoAjQhtQdBMCG2ByC1ByC2B2whtwcgtAcgtwdqIbgHILgHKAIQIbkHQQAhugcguQcgugdHIbsHQQEhvAcguwcgvAdxIb0HAkAgvQdFDQAgAygCWCG+ByC+BygCXCG/ByADKAI0IcAHQTAhwQcgwAcgwQdsIcIHIL8HIMIHaiHDByDDBygCECHEByADKAJYIcUHIMUHKAJYIcYHIMQHIMYHSyHHB0EBIcgHIMcHIMgHcSHJBwJAIMkHRQ0AQX8hygcgAyDKBzYCXAwECyADKAJYIcsHIMsHKAJUIcwHIAMoAlghzQcgzQcoAlwhzgcgAygCNCHPB0EwIdAHIM8HINAHbCHRByDOByDRB2oh0gcg0gcoAhAh0wdBASHUByDTByDUB2sh1QdBJCHWByDVByDWB2wh1wcgzAcg1wdqIdgHIAMoAlgh2Qcg2QcoAlwh2gcgAygCNCHbB0EwIdwHINsHINwHbCHdByDaByDdB2oh3gcg3gcg2Ac2AhALIAMoAlgh3wcg3wcoAlwh4AcgAygCNCHhB0EwIeIHIOEHIOIHbCHjByDgByDjB2oh5Acg5AcoAhgh5QdBACHmByDlByDmB0ch5wdBASHoByDnByDoB3Eh6QcCQCDpB0UNACADKAJYIeoHIOoHKAJcIesHIAMoAjQh7AdBMCHtByDsByDtB2wh7gcg6wcg7gdqIe8HIO8HKAIYIfAHIAMoAlgh8Qcg8QcoAlgh8gcg8Acg8gdLIfMHQQEh9Acg8wcg9AdxIfUHAkAg9QdFDQBBfyH2ByADIPYHNgJcDAQLIAMoAlgh9wcg9wcoAlQh+AcgAygCWCH5ByD5BygCXCH6ByADKAI0IfsHQTAh/Acg+wcg/AdsIf0HIPoHIP0HaiH+ByD+BygCGCH/B0EBIYAIIP8HIIAIayGBCEEkIYIIIIEIIIIIbCGDCCD4ByCDCGohhAggAygCWCGFCCCFCCgCXCGGCCADKAI0IYcIQTAhiAgghwggiAhsIYkIIIYIIIkIaiGKCCCKCCCECDYCGAsgAygCWCGLCCCLCCgCXCGMCCADKAI0IY0IQTAhjgggjQggjghsIY8IIIwIII8IaiGQCCCQCCgCCCGRCEEAIZIIIJEIIJIIRyGTCEEBIZQIIJMIIJQIcSGVCAJAIJUIRQ0AIAMoAlghlggglggoAlwhlwggAygCNCGYCEEwIZkIIJgIIJkIbCGaCCCXCCCaCGohmwggmwgoAgghnAggAygCWCGdCCCdCCgCaCGeCCCcCCCeCEshnwhBASGgCCCfCCCgCHEhoQgCQCChCEUNAEF/IaIIIAMgogg2AlwMBAsgAygCWCGjCCCjCCgCZCGkCCADKAJYIaUIIKUIKAJcIaYIIAMoAjQhpwhBMCGoCCCnCCCoCGwhqQggpgggqQhqIaoIIKoIKAIIIasIQQEhrAggqwggrAhrIa0IQSghrgggrQggrghsIa8IIKQIIK8IaiGwCCADKAJYIbEIILEIKAJcIbIIIAMoAjQhswhBMCG0CCCzCCC0CGwhtQggsgggtQhqIbYIILYIILAINgIICyADKAI0IbcIQQEhuAggtwgguAhqIbkIIAMguQg2AjQMAAsLQQAhugggAyC6CDYCMAJAA0AgAygCMCG7CCADKAJYIbwIILwIKAJYIb0IILsIIL0ISSG+CEEBIb8IIL4IIL8IcSHACCDACEUNASADKAJYIcEIIMEIKAJUIcIIIAMoAjAhwwhBJCHECCDDCCDECGwhxQggwgggxQhqIcYIIMYIKAIIIccIQQAhyAggxwggyAhHIckIQQEhygggyQggyghxIcsIAkAgywhFDQAgAygCWCHMCCDMCCgCVCHNCCADKAIwIc4IQSQhzwggzgggzwhsIdAIIM0IINAIaiHRCCDRCCgCCCHSCCADKAJYIdMIINMIKAJIIdQIINIIINQISyHVCEEBIdYIINUIINYIcSHXCAJAINcIRQ0AQX8h2AggAyDYCDYCXAwECyADKAJYIdkIINkIKAJEIdoIIAMoAlgh2wgg2wgoAlQh3AggAygCMCHdCEEkId4IIN0IIN4IbCHfCCDcCCDfCGoh4Agg4AgoAggh4QhBASHiCCDhCCDiCGsh4whB0AAh5Agg4wgg5AhsIeUIINoIIOUIaiHmCCADKAJYIecIIOcIKAJUIegIIAMoAjAh6QhBJCHqCCDpCCDqCGwh6wgg6Agg6whqIewIIOwIIOYINgIICyADKAIwIe0IQQEh7ggg7Qgg7ghqIe8IIAMg7wg2AjAMAAsLQQAh8AggAyDwCDYCLAJAA0AgAygCLCHxCCADKAJYIfIIIPIIKAI4IfMIIPEIIPMISSH0CEEBIfUIIPQIIPUIcSH2CCD2CEUNASADKAJYIfcIIPcIKAI0IfgIIAMoAiwh+QhBsAkh+ggg+Qgg+ghsIfsIIPgIIPsIaiH8CCD8CCgC/Ach/QhBACH+CCD9CCD+CEch/whBASGACSD/CCCACXEhgQkCQCCBCUUNACADKAJYIYIJIIIJKAI0IYMJIAMoAiwhhAlBsAkhhQkghAkghQlsIYYJIIMJIIYJaiGHCSCHCSgC/AchiAkgAygCWCGJCSCJCSgCYCGKCSCICSCKCUshiwlBASGMCSCLCSCMCXEhjQkCQCCNCUUNAEF/IY4JIAMgjgk2AlwMBAsgAygCWCGPCSCPCSgCXCGQCSADKAJYIZEJIJEJKAI0IZIJIAMoAiwhkwlBsAkhlAkgkwkglAlsIZUJIJIJIJUJaiGWCSCWCSgC/AchlwlBASGYCSCXCSCYCWshmQlBMCGaCSCZCSCaCWwhmwkgkAkgmwlqIZwJIAMoAlghnQkgnQkoAjQhngkgAygCLCGfCUGwCSGgCSCfCSCgCWwhoQkgngkgoQlqIaIJIKIJIJwJNgL8BwsgAygCWCGjCSCjCSgCNCGkCSADKAIsIaUJQbAJIaYJIKUJIKYJbCGnCSCkCSCnCWohqAkgqAkoAtQIIakJQQAhqgkgqQkgqglHIasJQQEhrAkgqwkgrAlxIa0JAkAgrQlFDQAgAygCWCGuCSCuCSgCNCGvCSADKAIsIbAJQbAJIbEJILAJILEJbCGyCSCvCSCyCWohswkgswkoAtQIIbQJIAMoAlghtQkgtQkoAmAhtgkgtAkgtglLIbcJQQEhuAkgtwkguAlxIbkJAkAguQlFDQBBfyG6CSADILoJNgJcDAQLIAMoAlghuwkguwkoAlwhvAkgAygCWCG9CSC9CSgCNCG+CSADKAIsIb8JQbAJIcAJIL8JIMAJbCHBCSC+CSDBCWohwgkgwgkoAtQIIcMJQQEhxAkgwwkgxAlrIcUJQTAhxgkgxQkgxglsIccJILwJIMcJaiHICSADKAJYIckJIMkJKAI0IcoJIAMoAiwhywlBsAkhzAkgywkgzAlsIc0JIMoJIM0JaiHOCSDOCSDICTYC1AgLIAMoAlghzwkgzwkoAjQh0AkgAygCLCHRCUGwCSHSCSDRCSDSCWwh0wkg0Akg0wlqIdQJINQJKAKoCCHVCUEAIdYJINUJINYJRyHXCUEBIdgJINcJINgJcSHZCQJAINkJRQ0AIAMoAlgh2gkg2gkoAjQh2wkgAygCLCHcCUGwCSHdCSDcCSDdCWwh3gkg2wkg3glqId8JIN8JKAKoCCHgCSADKAJYIeEJIOEJKAJgIeIJIOAJIOIJSyHjCUEBIeQJIOMJIOQJcSHlCQJAIOUJRQ0AQX8h5gkgAyDmCTYCXAwECyADKAJYIecJIOcJKAJcIegJIAMoAlgh6Qkg6QkoAjQh6gkgAygCLCHrCUGwCSHsCSDrCSDsCWwh7Qkg6gkg7QlqIe4JIO4JKAKoCCHvCUEBIfAJIO8JIPAJayHxCUEwIfIJIPEJIPIJbCHzCSDoCSDzCWoh9AkgAygCWCH1CSD1CSgCNCH2CSADKAIsIfcJQbAJIfgJIPcJIPgJbCH5CSD2CSD5CWoh+gkg+gkg9Ak2AqgICyADKAJYIfsJIPsJKAI0IfwJIAMoAiwh/QlBsAkh/gkg/Qkg/glsIf8JIPwJIP8JaiGACiCACigCOCGBCkEAIYIKIIEKIIIKRyGDCkEBIYQKIIMKIIQKcSGFCgJAIIUKRQ0AIAMoAlghhgoghgooAjQhhwogAygCLCGICkGwCSGJCiCICiCJCmwhigoghwogigpqIYsKIIsKKAI4IYwKIAMoAlghjQogjQooAmAhjgogjAogjgpLIY8KQQEhkAogjwogkApxIZEKAkAgkQpFDQBBfyGSCiADIJIKNgJcDAQLIAMoAlghkwogkwooAlwhlAogAygCWCGVCiCVCigCNCGWCiADKAIsIZcKQbAJIZgKIJcKIJgKbCGZCiCWCiCZCmohmgogmgooAjghmwpBASGcCiCbCiCcCmshnQpBMCGeCiCdCiCeCmwhnwoglAognwpqIaAKIAMoAlghoQogoQooAjQhogogAygCLCGjCkGwCSGkCiCjCiCkCmwhpQogogogpQpqIaYKIKYKIKAKNgI4CyADKAJYIacKIKcKKAI0IagKIAMoAiwhqQpBsAkhqgogqQogqgpsIasKIKgKIKsKaiGsCiCsCigCZCGtCkEAIa4KIK0KIK4KRyGvCkEBIbAKIK8KILAKcSGxCgJAILEKRQ0AIAMoAlghsgogsgooAjQhswogAygCLCG0CkGwCSG1CiC0CiC1CmwhtgogswogtgpqIbcKILcKKAJkIbgKIAMoAlghuQoguQooAmAhugoguAogugpLIbsKQQEhvAoguwogvApxIb0KAkAgvQpFDQBBfyG+CiADIL4KNgJcDAQLIAMoAlghvwogvwooAlwhwAogAygCWCHBCiDBCigCNCHCCiADKAIsIcMKQbAJIcQKIMMKIMQKbCHFCiDCCiDFCmohxgogxgooAmQhxwpBASHICiDHCiDICmshyQpBMCHKCiDJCiDKCmwhywogwAogywpqIcwKIAMoAlghzQogzQooAjQhzgogAygCLCHPCkGwCSHQCiDPCiDQCmwh0Qogzgog0QpqIdIKINIKIMwKNgJkCyADKAJYIdMKINMKKAI0IdQKIAMoAiwh1QpBsAkh1gog1Qog1gpsIdcKINQKINcKaiHYCiDYCigCqAEh2QpBACHaCiDZCiDaCkch2wpBASHcCiDbCiDcCnEh3QoCQCDdCkUNACADKAJYId4KIN4KKAI0Id8KIAMoAiwh4ApBsAkh4Qog4Aog4QpsIeIKIN8KIOIKaiHjCiDjCigCqAEh5AogAygCWCHlCiDlCigCYCHmCiDkCiDmCksh5wpBASHoCiDnCiDoCnEh6QoCQCDpCkUNAEF/IeoKIAMg6go2AlwMBAsgAygCWCHrCiDrCigCXCHsCiADKAJYIe0KIO0KKAI0Ie4KIAMoAiwh7wpBsAkh8Aog7wog8ApsIfEKIO4KIPEKaiHyCiDyCigCqAEh8wpBASH0CiDzCiD0Cmsh9QpBMCH2CiD1CiD2Cmwh9wog7Aog9wpqIfgKIAMoAlgh+Qog+QooAjQh+gogAygCLCH7CkGwCSH8CiD7CiD8Cmwh/Qog+gog/QpqIf4KIP4KIPgKNgKoAQsgAygCWCH/CiD/CigCNCGACyADKAIsIYELQbAJIYILIIELIIILbCGDCyCACyCDC2ohhAsghAsoAtQBIYULQQAhhgsghQsghgtHIYcLQQEhiAsghwsgiAtxIYkLAkAgiQtFDQAgAygCWCGKCyCKCygCNCGLCyADKAIsIYwLQbAJIY0LIIwLII0LbCGOCyCLCyCOC2ohjwsgjwsoAtQBIZALIAMoAlghkQsgkQsoAmAhkgsgkAsgkgtLIZMLQQEhlAsgkwsglAtxIZULAkAglQtFDQBBfyGWCyADIJYLNgJcDAQLIAMoAlghlwsglwsoAlwhmAsgAygCWCGZCyCZCygCNCGaCyADKAIsIZsLQbAJIZwLIJsLIJwLbCGdCyCaCyCdC2ohngsgngsoAtQBIZ8LQQEhoAsgnwsgoAtrIaELQTAhogsgoQsgogtsIaMLIJgLIKMLaiGkCyADKAJYIaULIKULKAI0IaYLIAMoAiwhpwtBsAkhqAsgpwsgqAtsIakLIKYLIKkLaiGqCyCqCyCkCzYC1AELIAMoAlghqwsgqwsoAjQhrAsgAygCLCGtC0GwCSGuCyCtCyCuC2whrwsgrAsgrwtqIbALILALKAKgAiGxC0EAIbILILELILILRyGzC0EBIbQLILMLILQLcSG1CwJAILULRQ0AIAMoAlghtgsgtgsoAjQhtwsgAygCLCG4C0GwCSG5CyC4CyC5C2whugsgtwsgugtqIbsLILsLKAKgAiG8CyADKAJYIb0LIL0LKAJgIb4LILwLIL4LSyG/C0EBIcALIL8LIMALcSHBCwJAIMELRQ0AQX8hwgsgAyDCCzYCXAwECyADKAJYIcMLIMMLKAJcIcQLIAMoAlghxQsgxQsoAjQhxgsgAygCLCHHC0GwCSHICyDHCyDIC2whyQsgxgsgyQtqIcoLIMoLKAKgAiHLC0EBIcwLIMsLIMwLayHNC0EwIc4LIM0LIM4LbCHPCyDECyDPC2oh0AsgAygCWCHRCyDRCygCNCHSCyADKAIsIdMLQbAJIdQLINMLINQLbCHVCyDSCyDVC2oh1gsg1gsg0As2AqACCyADKAJYIdcLINcLKAI0IdgLIAMoAiwh2QtBsAkh2gsg2Qsg2gtsIdsLINgLINsLaiHcCyDcCygCzAIh3QtBACHeCyDdCyDeC0ch3wtBASHgCyDfCyDgC3Eh4QsCQCDhC0UNACADKAJYIeILIOILKAI0IeMLIAMoAiwh5AtBsAkh5Qsg5Asg5QtsIeYLIOMLIOYLaiHnCyDnCygCzAIh6AsgAygCWCHpCyDpCygCYCHqCyDoCyDqC0sh6wtBASHsCyDrCyDsC3Eh7QsCQCDtC0UNAEF/Ie4LIAMg7gs2AlwMBAsgAygCWCHvCyDvCygCXCHwCyADKAJYIfELIPELKAI0IfILIAMoAiwh8wtBsAkh9Asg8wsg9AtsIfULIPILIPULaiH2CyD2CygCzAIh9wtBASH4CyD3CyD4C2sh+QtBMCH6CyD5CyD6C2wh+wsg8Asg+wtqIfwLIAMoAlgh/Qsg/QsoAjQh/gsgAygCLCH/C0GwCSGADCD/CyCADGwhgQwg/gsggQxqIYIMIIIMIPwLNgLMAgsgAygCWCGDDCCDDCgCNCGEDCADKAIsIYUMQbAJIYYMIIUMIIYMbCGHDCCEDCCHDGohiAwgiAwoAvgCIYkMQQAhigwgiQwgigxHIYsMQQEhjAwgiwwgjAxxIY0MAkAgjQxFDQAgAygCWCGODCCODCgCNCGPDCADKAIsIZAMQbAJIZEMIJAMIJEMbCGSDCCPDCCSDGohkwwgkwwoAvgCIZQMIAMoAlghlQwglQwoAmAhlgwglAwglgxLIZcMQQEhmAwglwwgmAxxIZkMAkAgmQxFDQBBfyGaDCADIJoMNgJcDAQLIAMoAlghmwwgmwwoAlwhnAwgAygCWCGdDCCdDCgCNCGeDCADKAIsIZ8MQbAJIaAMIJ8MIKAMbCGhDCCeDCChDGohogwgogwoAvgCIaMMQQEhpAwgowwgpAxrIaUMQTAhpgwgpQwgpgxsIacMIJwMIKcMaiGoDCADKAJYIakMIKkMKAI0IaoMIAMoAiwhqwxBsAkhrAwgqwwgrAxsIa0MIKoMIK0MaiGuDCCuDCCoDDYC+AILIAMoAlghrwwgrwwoAjQhsAwgAygCLCGxDEGwCSGyDCCxDCCyDGwhswwgsAwgswxqIbQMILQMKAKwAyG1DEEAIbYMILUMILYMRyG3DEEBIbgMILcMILgMcSG5DAJAILkMRQ0AIAMoAlghugwgugwoAjQhuwwgAygCLCG8DEGwCSG9DCC8DCC9DGwhvgwguwwgvgxqIb8MIL8MKAKwAyHADCADKAJYIcEMIMEMKAJgIcIMIMAMIMIMSyHDDEEBIcQMIMMMIMQMcSHFDAJAIMUMRQ0AQX8hxgwgAyDGDDYCXAwECyADKAJYIccMIMcMKAJcIcgMIAMoAlghyQwgyQwoAjQhygwgAygCLCHLDEGwCSHMDCDLDCDMDGwhzQwgygwgzQxqIc4MIM4MKAKwAyHPDEEBIdAMIM8MINAMayHRDEEwIdIMINEMINIMbCHTDCDIDCDTDGoh1AwgAygCWCHVDCDVDCgCNCHWDCADKAIsIdcMQbAJIdgMINcMINgMbCHZDCDWDCDZDGoh2gwg2gwg1Aw2ArADCyADKAJYIdsMINsMKAI0IdwMIAMoAiwh3QxBsAkh3gwg3Qwg3gxsId8MINwMIN8MaiHgDCDgDCgC3AMh4QxBACHiDCDhDCDiDEch4wxBASHkDCDjDCDkDHEh5QwCQCDlDEUNACADKAJYIeYMIOYMKAI0IecMIAMoAiwh6AxBsAkh6Qwg6Awg6QxsIeoMIOcMIOoMaiHrDCDrDCgC3AMh7AwgAygCWCHtDCDtDCgCYCHuDCDsDCDuDEsh7wxBASHwDCDvDCDwDHEh8QwCQCDxDEUNAEF/IfIMIAMg8gw2AlwMBAsgAygCWCHzDCDzDCgCXCH0DCADKAJYIfUMIPUMKAI0IfYMIAMoAiwh9wxBsAkh+Awg9wwg+AxsIfkMIPYMIPkMaiH6DCD6DCgC3AMh+wxBASH8DCD7DCD8DGsh/QxBMCH+DCD9DCD+DGwh/wwg9Awg/wxqIYANIAMoAlghgQ0ggQ0oAjQhgg0gAygCLCGDDUGwCSGEDSCDDSCEDWwhhQ0ggg0ghQ1qIYYNIIYNIIANNgLcAwsgAygCWCGHDSCHDSgCNCGIDSADKAIsIYkNQbAJIYoNIIkNIIoNbCGLDSCIDSCLDWohjA0gjA0oAoAFIY0NQQAhjg0gjQ0gjg1HIY8NQQEhkA0gjw0gkA1xIZENAkAgkQ1FDQAgAygCWCGSDSCSDSgCNCGTDSADKAIsIZQNQbAJIZUNIJQNIJUNbCGWDSCTDSCWDWohlw0glw0oAoAFIZgNIAMoAlghmQ0gmQ0oAmAhmg0gmA0gmg1LIZsNQQEhnA0gmw0gnA1xIZ0NAkAgnQ1FDQBBfyGeDSADIJ4NNgJcDAQLIAMoAlghnw0gnw0oAlwhoA0gAygCWCGhDSChDSgCNCGiDSADKAIsIaMNQbAJIaQNIKMNIKQNbCGlDSCiDSClDWohpg0gpg0oAoAFIacNQQEhqA0gpw0gqA1rIakNQTAhqg0gqQ0gqg1sIasNIKANIKsNaiGsDSADKAJYIa0NIK0NKAI0Ia4NIAMoAiwhrw1BsAkhsA0grw0gsA1sIbENIK4NILENaiGyDSCyDSCsDTYCgAULIAMoAlghsw0gsw0oAjQhtA0gAygCLCG1DUGwCSG2DSC1DSC2DWwhtw0gtA0gtw1qIbgNILgNKAKwBSG5DUEAIboNILkNILoNRyG7DUEBIbwNILsNILwNcSG9DQJAIL0NRQ0AIAMoAlghvg0gvg0oAjQhvw0gAygCLCHADUGwCSHBDSDADSDBDWwhwg0gvw0gwg1qIcMNIMMNKAKwBSHEDSADKAJYIcUNIMUNKAJgIcYNIMQNIMYNSyHHDUEBIcgNIMcNIMgNcSHJDQJAIMkNRQ0AQX8hyg0gAyDKDTYCXAwECyADKAJYIcsNIMsNKAJcIcwNIAMoAlghzQ0gzQ0oAjQhzg0gAygCLCHPDUGwCSHQDSDPDSDQDWwh0Q0gzg0g0Q1qIdINININKAKwBSHTDUEBIdQNINMNINQNayHVDUEwIdYNINUNINYNbCHXDSDMDSDXDWoh2A0gAygCWCHZDSDZDSgCNCHaDSADKAIsIdsNQbAJIdwNINsNINwNbCHdDSDaDSDdDWoh3g0g3g0g2A02ArAFCyADKAJYId8NIN8NKAI0IeANIAMoAiwh4Q1BsAkh4g0g4Q0g4g1sIeMNIOANIOMNaiHkDSDkDSgCmAQh5Q1BACHmDSDlDSDmDUch5w1BASHoDSDnDSDoDXEh6Q0CQCDpDUUNACADKAJYIeoNIOoNKAI0IesNIAMoAiwh7A1BsAkh7Q0g7A0g7Q1sIe4NIOsNIO4NaiHvDSDvDSgCmAQh8A0gAygCWCHxDSDxDSgCYCHyDSDwDSDyDUsh8w1BASH0DSDzDSD0DXEh9Q0CQCD1DUUNAEF/IfYNIAMg9g02AlwMBAsgAygCWCH3DSD3DSgCXCH4DSADKAJYIfkNIPkNKAI0IfoNIAMoAiwh+w1BsAkh/A0g+w0g/A1sIf0NIPoNIP0NaiH+DSD+DSgCmAQh/w1BASGADiD/DSCADmshgQ5BMCGCDiCBDiCCDmwhgw4g+A0ggw5qIYQOIAMoAlghhQ4ghQ4oAjQhhg4gAygCLCGHDkGwCSGIDiCHDiCIDmwhiQ4ghg4giQ5qIYoOIIoOIIQONgKYBAsgAygCWCGLDiCLDigCNCGMDiADKAIsIY0OQbAJIY4OII0OII4ObCGPDiCMDiCPDmohkA4gkA4oAtAEIZEOQQAhkg4gkQ4gkg5HIZMOQQEhlA4gkw4glA5xIZUOAkAglQ5FDQAgAygCWCGWDiCWDigCNCGXDiADKAIsIZgOQbAJIZkOIJgOIJkObCGaDiCXDiCaDmohmw4gmw4oAtAEIZwOIAMoAlghnQ4gnQ4oAmAhng4gnA4gng5LIZ8OQQEhoA4gnw4goA5xIaEOAkAgoQ5FDQBBfyGiDiADIKIONgJcDAQLIAMoAlghow4gow4oAlwhpA4gAygCWCGlDiClDigCNCGmDiADKAIsIacOQbAJIagOIKcOIKgObCGpDiCmDiCpDmohqg4gqg4oAtAEIasOQQEhrA4gqw4grA5rIa0OQTAhrg4grQ4grg5sIa8OIKQOIK8OaiGwDiADKAJYIbEOILEOKAI0IbIOIAMoAiwhsw5BsAkhtA4gsw4gtA5sIbUOILIOILUOaiG2DiC2DiCwDjYC0AQLIAMoAlghtw4gtw4oAjQhuA4gAygCLCG5DkGwCSG6DiC5DiC6Dmwhuw4guA4guw5qIbwOILwOKAL4BSG9DkEAIb4OIL0OIL4ORyG/DkEBIcAOIL8OIMAOcSHBDgJAIMEORQ0AIAMoAlghwg4gwg4oAjQhww4gAygCLCHEDkGwCSHFDiDEDiDFDmwhxg4gww4gxg5qIccOIMcOKAL4BSHIDiADKAJYIckOIMkOKAJgIcoOIMgOIMoOSyHLDkEBIcwOIMsOIMwOcSHNDgJAIM0ORQ0AQX8hzg4gAyDODjYCXAwECyADKAJYIc8OIM8OKAJcIdAOIAMoAlgh0Q4g0Q4oAjQh0g4gAygCLCHTDkGwCSHUDiDTDiDUDmwh1Q4g0g4g1Q5qIdYOINYOKAL4BSHXDkEBIdgOINcOINgOayHZDkEwIdoOINkOINoObCHbDiDQDiDbDmoh3A4gAygCWCHdDiDdDigCNCHeDiADKAIsId8OQbAJIeAOIN8OIOAObCHhDiDeDiDhDmoh4g4g4g4g3A42AvgFCyADKAJYIeMOIOMOKAI0IeQOIAMoAiwh5Q5BsAkh5g4g5Q4g5g5sIecOIOQOIOcOaiHoDiDoDigCsAYh6Q5BACHqDiDpDiDqDkch6w5BASHsDiDrDiDsDnEh7Q4CQCDtDkUNACADKAJYIe4OIO4OKAI0Ie8OIAMoAiwh8A5BsAkh8Q4g8A4g8Q5sIfIOIO8OIPIOaiHzDiDzDigCsAYh9A4gAygCWCH1DiD1DigCYCH2DiD0DiD2Dksh9w5BASH4DiD3DiD4DnEh+Q4CQCD5DkUNAEF/IfoOIAMg+g42AlwMBAsgAygCWCH7DiD7DigCXCH8DiADKAJYIf0OIP0OKAI0If4OIAMoAiwh/w5BsAkhgA8g/w4ggA9sIYEPIP4OIIEPaiGCDyCCDygCsAYhgw9BASGEDyCDDyCED2shhQ9BMCGGDyCFDyCGD2whhw8g/A4ghw9qIYgPIAMoAlghiQ8giQ8oAjQhig8gAygCLCGLD0GwCSGMDyCLDyCMD2whjQ8gig8gjQ9qIY4PII4PIIgPNgKwBgsgAygCWCGPDyCPDygCNCGQDyADKAIsIZEPQbAJIZIPIJEPIJIPbCGTDyCQDyCTD2ohlA8glA8oAtwGIZUPQQAhlg8glQ8glg9HIZcPQQEhmA8glw8gmA9xIZkPAkAgmQ9FDQAgAygCWCGaDyCaDygCNCGbDyADKAIsIZwPQbAJIZ0PIJwPIJ0PbCGeDyCbDyCeD2ohnw8gnw8oAtwGIaAPIAMoAlghoQ8goQ8oAmAhog8goA8gog9LIaMPQQEhpA8gow8gpA9xIaUPAkAgpQ9FDQBBfyGmDyADIKYPNgJcDAQLIAMoAlghpw8gpw8oAlwhqA8gAygCWCGpDyCpDygCNCGqDyADKAIsIasPQbAJIawPIKsPIKwPbCGtDyCqDyCtD2ohrg8grg8oAtwGIa8PQQEhsA8grw8gsA9rIbEPQTAhsg8gsQ8gsg9sIbMPIKgPILMPaiG0DyADKAJYIbUPILUPKAI0IbYPIAMoAiwhtw9BsAkhuA8gtw8guA9sIbkPILYPILkPaiG6DyC6DyC0DzYC3AYLIAMoAlghuw8guw8oAjQhvA8gAygCLCG9D0GwCSG+DyC9DyC+D2whvw8gvA8gvw9qIcAPIMAPKAKYByHBD0EAIcIPIMEPIMIPRyHDD0EBIcQPIMMPIMQPcSHFDwJAIMUPRQ0AIAMoAlghxg8gxg8oAjQhxw8gAygCLCHID0GwCSHJDyDIDyDJD2whyg8gxw8gyg9qIcsPIMsPKAKYByHMDyADKAJYIc0PIM0PKAJgIc4PIMwPIM4PSyHPD0EBIdAPIM8PINAPcSHRDwJAINEPRQ0AQX8h0g8gAyDSDzYCXAwECyADKAJYIdMPINMPKAJcIdQPIAMoAlgh1Q8g1Q8oAjQh1g8gAygCLCHXD0GwCSHYDyDXDyDYD2wh2Q8g1g8g2Q9qIdoPINoPKAKYByHbD0EBIdwPINsPINwPayHdD0EwId4PIN0PIN4PbCHfDyDUDyDfD2oh4A8gAygCWCHhDyDhDygCNCHiDyADKAIsIeMPQbAJIeQPIOMPIOQPbCHlDyDiDyDlD2oh5g8g5g8g4A82ApgHCyADKAJYIecPIOcPKAI0IegPIAMoAiwh6Q9BsAkh6g8g6Q8g6g9sIesPIOgPIOsPaiHsDyDsDygCzAch7Q9BACHuDyDtDyDuD0ch7w9BASHwDyDvDyDwD3Eh8Q8CQCDxD0UNACADKAJYIfIPIPIPKAI0IfMPIAMoAiwh9A9BsAkh9Q8g9A8g9Q9sIfYPIPMPIPYPaiH3DyD3DygCzAch+A8gAygCWCH5DyD5DygCYCH6DyD4DyD6D0sh+w9BASH8DyD7DyD8D3Eh/Q8CQCD9D0UNAEF/If4PIAMg/g82AlwMBAsgAygCWCH/DyD/DygCXCGAECADKAJYIYEQIIEQKAI0IYIQIAMoAiwhgxBBsAkhhBAggxAghBBsIYUQIIIQIIUQaiGGECCGECgCzAchhxBBASGIECCHECCIEGshiRBBMCGKECCJECCKEGwhixAggBAgixBqIYwQIAMoAlghjRAgjRAoAjQhjhAgAygCLCGPEEGwCSGQECCPECCQEGwhkRAgjhAgkRBqIZIQIJIQIIwQNgLMBwsgAygCLCGTEEEBIZQQIJMQIJQQaiGVECADIJUQNgIsDAALC0EAIZYQIAMglhA2AigCQANAIAMoAighlxAgAygCWCGYECCYECgCSCGZECCXECCZEEkhmhBBASGbECCaECCbEHEhnBAgnBBFDQEgAygCWCGdECCdECgCRCGeECADKAIoIZ8QQdAAIaAQIJ8QIKAQbCGhECCeECChEGohohAgohAoAgQhoxBBACGkECCjECCkEEchpRBBASGmECClECCmEHEhpxACQAJAIKcQRQ0AIAMoAlghqBAgqBAoAkQhqRAgAygCKCGqEEHQACGrECCqECCrEGwhrBAgqRAgrBBqIa0QIK0QKAIEIa4QIAMoAlghrxAgrxAoAlAhsBAgrhAgsBBLIbEQQQEhshAgsRAgshBxIbMQILMQRQ0BC0F/IbQQIAMgtBA2AlwMAwsgAygCWCG1ECC1ECgCTCG2ECADKAJYIbcQILcQKAJEIbgQIAMoAighuRBB0AAhuhAguRAguhBsIbsQILgQILsQaiG8ECC8ECgCBCG9EEEBIb4QIL0QIL4QayG/EEEoIcAQIL8QIMAQbCHBECC2ECDBEGohwhAgAygCWCHDECDDECgCRCHEECADKAIoIcUQQdAAIcYQIMUQIMYQbCHHECDEECDHEGohyBAgyBAgwhA2AgQgAygCWCHJECDJECgCRCHKECADKAIoIcsQQdAAIcwQIMsQIMwQbCHNECDKECDNEGohzhAgzhAoAhwhzxACQCDPEEUNACADKAJYIdAQINAQKAJEIdEQIAMoAigh0hBB0AAh0xAg0hAg0xBsIdQQINEQINQQaiHVECDVECgCICHWEEEAIdcQINYQINcQRyHYEEEBIdkQINgQINkQcSHaEAJAAkAg2hBFDQAgAygCWCHbECDbECgCRCHcECADKAIoId0QQdAAId4QIN0QIN4QbCHfECDcECDfEGoh4BAg4BAoAiAh4RAgAygCWCHiECDiECgCUCHjECDhECDjEEsh5BBBASHlECDkECDlEHEh5hAg5hBFDQELQX8h5xAgAyDnEDYCXAwECyADKAJYIegQIOgQKAJMIekQIAMoAlgh6hAg6hAoAkQh6xAgAygCKCHsEEHQACHtECDsECDtEGwh7hAg6xAg7hBqIe8QIO8QKAIgIfAQQQEh8RAg8BAg8RBrIfIQQSgh8xAg8hAg8xBsIfQQIOkQIPQQaiH1ECADKAJYIfYQIPYQKAJEIfcQIAMoAigh+BBB0AAh+RAg+BAg+RBsIfoQIPcQIPoQaiH7ECD7ECD1EDYCIAsgAygCKCH8EEEBIf0QIPwQIP0QaiH+ECADIP4QNgIoDAALC0EAIf8QIAMg/xA2AiQCQANAIAMoAiQhgBEgAygCWCGBESCBESgCcCGCESCAESCCEUkhgxFBASGEESCDESCEEXEhhREghRFFDQFBACGGESADIIYRNgIgAkADQCADKAIgIYcRIAMoAlghiBEgiBEoAmwhiREgAygCJCGKEUEoIYsRIIoRIIsRbCGMESCJESCMEWohjREgjREoAgghjhEghxEgjhFJIY8RQQEhkBEgjxEgkBFxIZERIJERRQ0BIAMoAlghkhEgkhEoAmwhkxEgAygCJCGUEUEoIZURIJQRIJURbCGWESCTESCWEWohlxEglxEoAgQhmBEgAygCICGZEUECIZoRIJkRIJoRdCGbESCYESCbEWohnBEgnBEoAgAhnRFBACGeESCdESCeEUchnxFBASGgESCfESCgEXEhoRECQAJAIKERRQ0AIAMoAlghohEgohEoAmwhoxEgAygCJCGkEUEoIaURIKQRIKURbCGmESCjESCmEWohpxEgpxEoAgQhqBEgAygCICGpEUECIaoRIKkRIKoRdCGrESCoESCrEWohrBEgrBEoAgAhrREgAygCWCGuESCuESgCiAEhrxEgrREgrxFLIbARQQEhsREgsBEgsRFxIbIRILIRRQ0BC0F/IbMRIAMgsxE2AlwMBQsgAygCWCG0ESC0ESgChAEhtREgAygCWCG2ESC2ESgCbCG3ESADKAIkIbgRQSghuREguBEguRFsIboRILcRILoRaiG7ESC7ESgCBCG8ESADKAIgIb0RQQIhvhEgvREgvhF0Ib8RILwRIL8RaiHAESDAESgCACHBEUEBIcIRIMERIMIRayHDEUHAASHEESDDESDEEWwhxREgtREgxRFqIcYRIAMoAlghxxEgxxEoAmwhyBEgAygCJCHJEUEoIcoRIMkRIMoRbCHLESDIESDLEWohzBEgzBEoAgQhzREgAygCICHOEUECIc8RIM4RIM8RdCHQESDNESDQEWoh0REg0REgxhE2AgAgAygCICHSEUEBIdMRINIRINMRaiHUESADINQRNgIgDAALCyADKAJYIdURINURKAJsIdYRIAMoAiQh1xFBKCHYESDXESDYEWwh2REg1hEg2RFqIdoRINoRKAIMIdsRQQAh3BEg2xEg3BFHId0RQQEh3hEg3REg3hFxId8RAkAg3xFFDQAgAygCWCHgESDgESgCbCHhESADKAIkIeIRQSgh4xEg4hEg4xFsIeQRIOERIOQRaiHlESDlESgCDCHmESADKAJYIecRIOcRKAKIASHo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ASCAASCBARCNgYCAACGCASAHKAIYIYMBIIMBIIIBNgIMIAcoAiAhhAFBASGFASCEASCFAWohhgEgByCGATYCIAwBCyAHKAIkIYcBIAcoAiAhiAFBFCGJASCIASCJAWwhigEghwEgigFqIYsBIAcoAhwhjAFBiZSEgAAhjQEgiwEgjAEgjQEQ2oCAgAAhjgECQAJAII4BDQAgBygCICGPAUEBIZABII8BIJABaiGRASAHIJEBNgIgIAcoAiQhkgEgBygCICGTAUEUIZQBIJMBIJQBbCGVASCSASCVAWohlgEgBygCHCGXASCWASCXARCNgYCAACGYASAHKAIYIZkBIJkBIJgBNgIQIAcoAiAhmgFBASGbASCaASCbAWohnAEgByCcATYCIAwBCyAHKAIkIZ0BIAcoAiAhngFBFCGfASCeASCfAWwhoAEgnQEgoAFqIaEBIAcoAhwhogFB3YKEgAAhowEgoQEgogEgowEQ2oCAgAAhpAECQAJAIKQBDQAgBygCICGlAUEBIaYBIKUBIKYBaiGnASAHIKcBNgIgIAcoAiQhqAEgBygCICGpAUEUIaoBIKkBIKoBbCGrASCoASCrAWohrAEgBygCHCGtASCsASCtARDogICAACGuASAHIK4BNgIMIAcoAgwhrwFB7u59IbABIK8BILABaiGxASCxASCmAUsaAkACQAJAAkAgsQEOAgABAgtBAiGyASAHILIBNgIMDAILQQEhswEgByCzATYCDAwBC0EAIbQBIAcgtAE2AgwLIAcoAgwhtQEgBygCGCG2ASC2ASC1ATYCFCAHKAIgIbcBQQEhuAEgtwEguAFqIbkBIAcguQE2AiAMAQsgBygCJCG6ASAHKAIgIbsBQRQhvAEguwEgvAFsIb0BILoBIL0BaiG+ASAHKAIcIb8BQbmFhIAAIcABIL4BIL8BIMABENqAgIAAIcEBAkACQCDBAQ0AIAcoAighwgEgBygCJCHDASAHKAIgIcQBQQEhxQEgxAEgxQFqIcYBIAcoAhwhxwEgBygCGCHIAUE8IckBIMgBIMkBaiHKASDCASDDASDGASDHASDKARDqgICAACHLASAHIMsBNgIgDAELIAcoAiQhzAEgBygCICHNAUEUIc4BIM0BIM4BbCHPASDMASDPAWoh0AEgBygCHCHRAUGchISAACHSASDQASDRASDSARDa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ASAHKAIMIYQBQbmFhIAAIYUBIIMBIIQBIIUBENqAgIAAIYYBAkACQCCGAQ0AIAcoAhghhwEgBygCFCGIASAHKAIQIYkBQQEhigEgiQEgigFqIYsBIAcoAgwhjAEgBygCCCGNAUEQIY4BII0BII4BaiGPASCHASCIASCLASCMASCPARDqgICAACGQASAHIJABNgIQDAELIAcoAhQhkQEgBygCECGSAUEUIZMBIJIBIJMBbCGUASCRASCUAWohlQEgBygCDCGWAUGchISAACGXASCVASCWASCXARDagICAACGYAQJAAkAgmAENACAHKAIYIZkBIAcoAhQhmgEgBygCECGbASAHKAIMIZwBIAcoAgghnQFBHCGeASCdASCeAWohnwEgBygCCCGgAUEgIaEBIKABIKEBaiGiASCZASCaASCbASCcASCfASCiARDzgICAACGjASAHIKMBNgIQDAELIAcoAhQhpAEgBygCECGlAUEBIaYBIKUBIKYBaiGnASCkASCnARDtgICAACGoASAHIKgBNgIQCwsLCwsLIAcoAhAhqQFBACGqASCpASCqAUghqwFBASGsASCrASCsAXEhrQECQCCtAUUNACAHKAIQIa4BIAcgrgE2AhwMAwsgBygCACGvAUEBIbABIK8BILABaiGxASAHILEBNgIADAALCyAHKAIQIbIBIAcgsgE2AhwLIAcoAhwhswFBICG0ASAHILQBaiG1ASC1ASSAgICAACCzAQ8LkiEBsAN/I4CAgIAAIQVBwAAhBiAFIAZrIQcgBySAgICAACAHIAA2AjggByABNgI0IAcgAjYCMCAHIAM2AiwgByAENgIoIAcoAjQhCCAHKAIwIQlBFCEKIAkgCmwhCyAIIAtqIQwgDCgCACENQQEhDiANIA5HIQ9BASEQIA8gEHEhEQJAAkAgEUUNAEF/IRIgByASNgI8DAELIAcoAjQhEyAHKAIwIRRBFCEVIBQgFWwhFiATIBZqIRcgFygCDCEYIAcgGDYCJCAHKAIwIRlBASEaIBkgGmohGyAHIBs2AjBBACEcIAcgHDYCIAJAA0AgBygCICEdIAcoAiQhHiAdIB5IIR9BASEgIB8gIHEhISAhRQ0BIAcoAjQhIiAHKAIwISNBFCEkICMgJGwhJSAiICVqISYgJigCACEnQQMhKCAnIChHISlBASEqICkgKnEh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FCGoASAHKAIQIakBQQEhqgEgqQEgqgFqIasBIAcoAgwhrAEgBygCCCGtAUEUIa4BIK0BIK4BaiGvASCnASCoASCrASCsASCvARDqgICAACGwASAHILABNgIQDAELIAcoAhQhsQEgBygCECGyAUEUIbMBILIBILMBbCG0ASCxASC0AWohtQEgBygCDCG2AUGchISAACG3ASC1ASC2ASC3ARDagICAACG4AQJAAkAguAENACAHKAIYIbkBIAcoAhQhugEgBygCECG7ASAHKAIMIbwBIAcoAgghvQFBICG+ASC9ASC+AWohvwEgBygCCCHAAUEkIcEBIMABIMEBaiHCASC5ASC6ASC7ASC8ASC/ASDCARDzgICAACHDASAHIMMBNgIQDAELIAcoAhQhxAEgBygCECHFAUEBIcYBIMUBIMYBaiHHASDEASDHARDtgICAACHIASAHIMgBNgIQCwsLCwsLCyAHKAIQIckBQQAhygEgyQEgygFIIcsBQQEhzAEgywEgzAFxIc0BAkAgzQFFDQAgBygCECHOASAHIM4BNgIcDAMLIAcoAgAhzwFBASHQASDPASDQAWoh0QEgByDRATYCAAwACwsgBygCECHSASAHINIBNgIcCyAHKAIcIdMBQSAh1AEgByDUAWoh1QEg1QEkgICAgAAg0wEPC/MRAfMBfyOAgICAACEFQTAhBiAFIAZrIQcgBySAgICAACAHIAA2AiggByABNgIkIAcgAjYCICAHIAM2AhwgByAENgIYIAcoAiQhCCAHKAIgIQlBFCEKIAkgCmwhCyAIIAtqIQwgDCgCACENQQEhDiANIA5HIQ9BASEQIA8gEHEhEQJAAkAgEUUNAEF/IRIgByASNgIsDAELIAcoAiQhEyAHKAIgIRRBFCEVIBQgFWwhFiATIBZqIRcgFygCDCEYIAcgGDYCFCAHKAIgIRlBASEaIBkgGmohGyAHIBs2AiBBACEcIAcgHDYCEAJAA0AgBygCECEdIAcoAhQhHiAdIB5IIR9BASEgIB8gIHEhISAhRQ0BIAcoAiQhIiAHKAIgISNBFCEkICMgJGwhJSAiICVqISYgJigCACEnQQMhKCAnIChHISlBASEqICkgKnEhKwJAAkAgKw0AIAcoAiQhLCAHKAIgIS1BFCEuIC0gLmwhLyAsIC9qITAgMCgCDCExIDENAQtBfyEy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NCHjASADKAI0IeQBQQQh5QEg5AEg5QFqIeYBIAMoAjQh5wFBHCHoASDnASDoAWoh6QEg4wEg5gEg6QEQwIGAgABBgAEh6gEgAyDqAWoh6wEg6wEkgICAgAAPC4tB0AIHfwF9AX8CfQF/AX0BfwJ9CH8BfQF/BH0BfwF9AX8FfQF/AX0BfwZ9AnwBfwF9A3wBfQN/An0BfwF9AX8BfQN/B30LfwF9AX8BfQF/AX0BfwR9AX8BfQF/Bn0GfwF9An8BfQJ/AX0BfwN9An8DfQJ/A30CfwN9AX8DfQF/A30BfwN9AX8BfQR/AX0BfwJ9AX8BfQN/B30LfwF9AX8BfQF/AX0BfwR9AX8BfQF/Bn0GfwF9An8BfQJ/AX0BfwN9An8DfQJ/A30CfwN9AX8DfQF/A30BfwN9AX8BfQt/AX0BfwF9AX8BfQF/BH0BfwF9AX8DfQF/AX0BfwR9An8BfQF/AX0BfwF9AX8FfQF/AX0BfwN9AX8BfQF/A30CfwF9AX8BfQF/AX0BfwR9AX8BfQF/BH0BfwF9AX8DfQJ/AX0BfwF9AX8BfQF/BX0BfwF9AX8EfQF/AX0BfwR9An8BfQF/An0RfwF9AX8BfQF/AX0BfwR9AX8BfQF/A30BfwF9AX8EfQF/AX0FfwJ+BX8BfQJ/AX0CfwF9An8BfQJ/BH0CfwN9An8DfQJ/A30CfwN9CH8BfQJ/AX0CfwF9BX8BfQV/AX0BfwF9AX8BfQF/BH0BfwF9AX8FfQd/A30CfwN9An8DfQJ/An0HfwF9AX8BfQF/AX0BfwR9AX8BfQF/Bn0EfwN9An8DfQJ/A30LfwF9AX8CfQJ/AX0BfwJ9An8BfQF/An0JfwF9AX8BfQF/AX0BfwV9AX8BfQF/AX0BfwF9AX8FfQF/AX0BfwF9AX8BfQF/BX0FfwF9An8BfQJ/AX0BfwN9B38DfQJ/A30CfwN9CX8BfQF/An0CfwF9AX8CfQJ/AX0BfwJ9C38BfQF/An0CfwF9AX8CfQJ/AX0BfwJ9Cn8jgICAgAAhAUHgBCECIAEgAmshAyADJICAgIAAIAMgADYCbEGgzYSAACEEIAQoAoABIQVBACEGIAYgBWshByAHsiEIIAMoAmwhCSAJKgKUASEKIAggCpQhCyADIAs4AmggBCgChAEhDCAMsiENIAMoAmwhDiAOKgKUASEPIA0gD5QhECADIBA4AmQgAygCbCERQQQhEiARIBJqIRNBHCEUIBEgFGohFSADIBM2AoABIAMgFTYCfCADKAKAASEWIAMoAnwhFyADIBY2ApwDIAMgFzYCmAMgAygCnAMhGCAYKgIAIRkgAygCmAMhGiAaKgIAIRsgGSAbkyEcIAMgHDgCqAMgAyoCqAMhHSAdIB2UIR4gAygCnAMhHyAfKgIEISAgAygCmAMhISAhKgIEISIgICAikyEjIAMgIzgCpAMgAyoCpAMhJCAkICSUISUgHiAlkiEmIAMoApwDIScgJyoCCCEoIAMoApgDISkgKSoCCCEqICggKpMhKyADICs4AqADIAMqAqADISwgLCAslCEtICYgLZIhLiAukSEvIC+7ITAgBCsDmAEhMSADKAJsITIgMioCmAEhMyAzuyE0IDEgNKIhNSA1IDCgITYgNrYhNyADIDc4AmBB0AAhOCADIDhqITkgOSE6IAMqAmQhO0MAAIA/ITwgAyA8OAIkQQAhPSA9siE+IAMgPjgCKEEAIT8gP7IhQCADIEA4AixBJCFBIAMgQWohQiBCIUMgAyA6NgLMASADIDs4AsgBIAMgQzYCxAEgAyoCyAEhREMAAAA/IUUgRCBFlCFGIAMgRjgCtAEgAyoCtAEhRyBHEPCBgIAAIUggAyBIOAKwASADKgK0ASFJIEkQpIKAgAAhSiADIEo4AqwBIAMoAsQBIUsgAyBLNgKwA0G4ASFMIAMgTGohTSBNIU4gAyBONgKsAyADKAKwAyFPIAMoAqwDIVAgAyBPNgK8AyADIFA2ArgDIAMoArwDIVEgAyBRNgLQAyADKALQAyFSIAMgUjYC1AMgAygC1AMhUyADKALUAyFUIAMgUzYC3AMgAyBUNgLYAyADKALcAyFVIFUqAgAhViADKALYAyFXIFcqAgAhWCADKALcAyFZIFkqAgQhWiADKALYAyFbIFsqAgQhXCBaIFyUIV0gViBYlCFeIF4gXZIhXyADKALcAyFgIGAqAgghYSADKALYAyFiIGIqAgghYyBhIGOUIWQgZCBfkiFlIGWRIWYgAyBmOAK0AyADKgK0AyFnQwAAADQhaCBnIGhdIWlBASFqIGkganEhawJAAkAga0UNACADKAK4AyFsIAMgbDYCwAMgAygCwAMhbUEAIW4gbrIhbyBtIG84AgggAygCwAMhcEEAIXEgcbIhciBwIHI4AgQgAygCwAMhc0EAIXQgdLIhdSBzIHU4AgAMAQsgAygCvAMhdiADKgK0AyF3QwAAgD8heCB4IHeVIXkgAygCuAMheiADIHY2AswDIAMgeTgCyAMgAyB6NgLEAyADKALMAyF7IHsqAgAhfCADKgLIAyF9IHwgfZQhfiADKALEAyF/IH8gfjgCACADKALMAyGAASCAASoCBCGBASADKgLIAyGCASCBASCCAZQhgwEgAygCxAMhhAEghAEggwE4AgQgAygCzAMhhQEghQEqAgghhgEgAyoCyAMhhwEghgEghwGUIYgBIAMoAsQDIYkBIIkBIIgBOAIICyADKgKsASGKASADKgK4ASGLASCKASCLAZQhjAEgAygCzAEhjQEgjQEgjAE4AgAgAyoCrAEhjgEgAyoCvAEhjwEgjgEgjwGUIZABIAMoAswBIZEBIJEBIJABOAIEIAMqAqwBIZIBIAMqAsABIZMBIJIBIJMBlCGUASADKALMASGVASCVASCUATgCCCADKgKwASGWASADKALMASGXASCXASCWATgCDEHAACGYASADIJgBaiGZASCZASGaASADKgJoIZsBQQAhnAEgnAGyIZ0BIAMgnQE4AhhDAACAPyGeASADIJ4BOAIcQQAhnwEgnwGyIaABIAMgoAE4AiBBGCGhASADIKEBaiGiASCiASGjASADIJoBNgKoASADIJsBOAKkASADIKMBNgKgASADKgKkASGkAUMAAAA/IaUBIKQBIKUBlCGmASADIKYBOAKMASADKgKMASGnASCnARDwgYCAACGoASADIKgBOAKIASADKgKMASGpASCpARCkgoCAACGqASADIKoBOAKEASADKAKgASGrASADIKsBNgLkA0GQASGsASADIKwBaiGtASCtASGuASADIK4BNgLgAyADKALkAyGvASADKALgAyGwASADIK8BNgLwAyADILABNgLsAyADKALwAyGxASADILEBNgKEBCADKAKEBCGyASADILIBNgKIBCADKAKIBCGzASADKAKIBCG0ASADILMBNgKQBCADILQBNgKMBCADKAKQBCG1ASC1ASoCACG2ASADKAKMBCG3ASC3ASoCACG4ASADKAKQBCG5ASC5ASoCBCG6ASADKAKMBCG7ASC7ASoCBCG8ASC6ASC8AZQhvQEgtgEguAGUIb4BIL4BIL0B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AygCbCHABUEEIcEFIMAFIMEFaiHCBSADILwFNgJ4IAMgvwU2AnQgAyDCBTYCcCADKAJ4IcMFIMMFKgIAIcQFIAMoAnQhxQUgxQUqAgAhxgUgxAUgxgWSIccFIAMoAnAhyAUgyAUgxwU4AgAgAygCeCHJBSDJBSoCBCHKBSADKAJ0IcsFIMsFKgIEIcwFIMoFIMwFkiHNBSADKAJwIc4FIM4FIM0FOAIEIAMoAnghzwUgzwUqAggh0AUgAygCdCHRBSDRBSoCCCHSBSDQBSDSBZIh0wUgAygCcCHUBSDUBSDTBTgCCCADKAJsIdUFIAMoAmwh1gVBBCHXBSDWBSDXBWoh2AUgAygCbCHZBUEcIdoFINkFINoFaiHbBSDVBSDYBSDbBRDAgYCAAEHgBCHcBSADINwFaiHdBSDdBSSAgICAAA8LjkqRAw9/AX0BfwJ9CX8BfQF/AX0BfwF9AX8EfQF/AX0BfwZ9Bn8BfQJ/AX0CfwF9AX8DfQJ/A30CfwN9An8DfQF/A30HfwN9An8DfQJ/A30BfwJ9B38DfQJ/A30CfwN9AX8BfQV/A30CfwN9An8DfQF/AX0HfwN9An8DfQJ/A30BfwF9B38DfQJ/A30CfwN9AX8BfQF/A30BfwN9AX8DfQF/A30BfwN9AX8DfQF/A30BfwN9AX8CfQJ/AX0CfwF9An8BfQJ/AX0CfwF9An8BfQF/AX0FfwF9AX8BfQR/AX0CfwF9An8BfQF/AX0JfwF9AX8BfQF/AX0BfwR9AX8BfQF/A30BfwF9AX8DfQF/AX0BfwF9AX8BfQF/BH0BfwF9AX8DfQF/AX0BfwN9AX8BfQF/AX0BfwF9AX8EfQF/AX0BfwN9AX8BfQF/A30BfwF9AX8BfQF/AX0BfwR9AX8BfQF/A30BfwF9AX8DfQV/AX0CfwF9An8BfQJ/AX0GfwF9An8BfQJ/AX0CfwF9AX8CfQl/AX0BfwF9AX8BfQF/BH0BfwF9AX8GfQZ/AX0CfwF9An8BfQF/A30CfwN9An8DfQJ/A30BfwN9B38DfQJ/A30CfwN9AX8CfQd/A30CfwN9An8DfQF/AX0FfwN9An8DfQJ/A30BfwF9B38DfQJ/A30CfwN9AX8BfQd/A30CfwN9An8DfQF/AX0BfwN9AX8DfQF/A30BfwN9AX8DfQF/A30BfwN9AX8DfQF/An0CfwF9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ATYCCCADIAE2AgQgAkF4aiABNgIAIAJBdGogATYCACAEQRlJDQAgAyABNgIYIAMgATYCFCADIAE2AhAgAyABNgIMIAJBcGogATYCACACQWxqIAE2AgAgAkFoaiABNgIAIAJBZGogATYCACAEIANBBHFBGHIiBWsiAkEgSQ0AIAGtQoGAgIAQfiEGIAMgBWohAQNAIAEgBjcDGCABIAY3AxAgASAGNwMIIAEgBjcDACABQSBqIQEgAkFgaiICQR9LDQALCyAACxEAIAAoAjwgASACEI6CgIAAC/8CAQd/I4CAgIAAQSBrIgMkgICAgAAgAyAAKAIcIgQ2AhAgACgCFCEFIAMgAjYCHCADIAE2AhggAyAFIARrIgE2AhQgASACaiEGIANBEGohBEECIQcCQAJAAkACQAJAIAAoAjwgA0EQakECIANBDGoQsYCAgAAQ24KAgABFDQAgBCEFDAELA0AgBiADKAIMIgFGDQICQCABQX9KDQAgBCEFDAQLIAQgASAEKAIEIghLIglBA3RqIgUgBSgCACABIAhBACAJG2siCGo2AgAgBEEMQQQgCRtqIgQgBCgCACAIazYCACAGIAFrIQYgBSEEIAAoAjwgBSAHIAlrIgcgA0EMahCxgICAABDbgoCAAEUNAAsLIAZBf0cNAQsgACAAKAIsIgE2AhwgACABNgIUIAAgASAAKAIwajYCECACIQEMAQtBACEBIABBADYCHCAAQgA3AxAgACAAKAIAQSByNgIAIAdBAkYNACACIAUoAgRrIQELIANBIGokgICAgAAgAQv2AQEEfyOAgICAAEEgayIDJICAgIAAIAMgATYCEEEAIQQgAyACIAAoAjAiBUEAR2s2AhQgACgCLCEGIAMgBTYCHCADIAY2AhhBICEFAkACQAJAIAAoAjwgA0EQakECIANBDGoQsoCAgAAQ24KAgAANACADKAIMIgVBAEoNAUEgQRAgBRshBQsgACAAKAIAIAVyNgIADAELIAUhBCAFIAMoAhQiBk0NACAAIAAoAiwiBDYCBCAAIAQgBSAGa2o2AggCQCAAKAIwRQ0AIAAgBEEBajYCBCABIAJqQX9qIAQtAAA6AAALIAIhBAsgA0EgaiSAgICAACAECwQAIAALGQAgACgCPBD8gYCAABCzgICAABDbgoCAAAuGAwECfyOAgICAAEEgayICJICAgIAAAkACQAJAAkBBwpWEgAAgASwAABCngoCAAA0AEOmBgIAAQRw2AgAMAQtBmAkQ4IKAgAAiAw0BC0EAIQMMAQsgA0EAQZABEPiBgIAAGgJAIAFBKxCngoCAAA0AIANBCEEEIAEtAABB8gBGGzYCAAsCQAJAIAEtAABB4QBGDQAgAygCACEBDAELAkAgAEEDQQAQr4CAgAAiAUGACHENACACIAFBgAhyrDcDECAAQQQgAkEQahCvgICAABoLIAMgAygCAEGAAXIiATYCAAsgA0F/NgJQIANBgAg2AjAgAyAANgI8IAMgA0GYAWo2AiwCQCABQQhxDQAgAiACQRhqrTcDACAAQZOoASACELCAgIAADQAgA0EKNgJQCyADQYyAgIAANgIoIANBjYCAgAA2AiQgA0GOgICAADYCICADQY+AgIAANgIMAkBBAC0AldGEgAANACADQX82AkwLIAMQlIKAgAAhAwsgAkEgaiSAgICAACADC50BAQN/I4CAgIAAQRBrIgIkgICAgAACQAJAAkBBwpWEgAAgASwAABCngoCAAA0AEOmBgIAAQRw2AgAMAQsgARD3gYCAACEDIAJCtgM3AwBBACEEQZx/IAAgA0GAgAJyIAIQroCAgAAQyYKAgAAiAEEASA0BIAAgARD+gYCAACIEDQEgABCzgICAABoLQQAhBAsgAkEQaiSAgICAACAECyQBAX8gABCtgoCAACECQX9BACACIABBASACIAEQjYKAgABHGwsTACACBEAgACABIAL8CgAACyAAC5EEAQN/AkAgAkGABEkNACAAIAEgAhCBgoCAAA8LIAAgAmohAwJAAkAgASAAc0EDcQ0AAkACQCAAQQNxDQAgACECDAELAkAgAg0AIAAhAgwBCyAAIQIDQCACIAEtAAA6AAAgAUEBaiEBIAJBAWoiAkEDcUUNASACIANJDQALCyADQXxxIQQCQCADQcAASQ0AIAIgBEFAaiIFSw0AA0AgAiABKAIANgIAIAIgASgCBDYCBCACIAEoAgg2AgggAiABKAIMNgIMIAIgASgCEDYCECACIAEoAhQ2AhQgAiABKAIYNgIYIAIgASgCHDYCHCACIAEoAiA2AiAgAiABKAIkNgIkIAIgASgCKDYCKCACIAEoAiw2AiwgAiABKAIwNgIwIAIgASgCNDYCNCACIAEoAjg2AjggAiABKAI8NgI8IAFBwABqIQEgAkHAAGoiAiAFTQ0ACwsgAiAETw0BA0AgAiABKAIANgIAIAFBBGohASACQQRqIgIgBEkNAAwCCwsCQCADQQRPDQAgACECDAELAkAgACADQXxqIgRNDQAgACECDAELIAAhAgNAIAIgAS0AADoAACACIAEtAAE6AAEgAiABLQACOgACIAIgAS0AAzoAAyABQQRqIQEgAkEEaiICIARNDQALCwJAIAIgA08NAANAIAIgAS0AADoAACABQQFqIQEgAkEBaiICIANHDQALCyAAC4kBAQJ/IAAgACgCSCIBQX9qIAFyNgJIAkAgACgCFCAAKAIcRg0AIABBAEEAIAAoAiQRhYCAgACAgICAABoLIABBADYCHCAAQgA3AxACQCAAKAIAIgFBBHFFDQAgACABQSByNgIAQX8PCyAAIAAoAiwgACgCMGoiAjYCCCAAIAI2AgQgAUEbdEEfdQuJAgEEfwJAAkAgAygCTEEATg0AQQEhBAwBCyADEPGBgIAARSEECyACIAFsIQUgAyADKAJIIgZBf2ogBnI2AkgCQAJAIAMoAgQiBiADKAIIIgdHDQAgBSEGDAELIAAgBiAHIAZrIgcgBSAHIAVJGyIHEIKCgIAAGiADIAMoAgQgB2o2AgQgBSAHayEGIAAgB2ohAAsCQCAGRQ0AA0ACQAJAIAMQg4KAgAANACADIAAgBiADKAIgEYWAgIAAgICAgAAiBw0BCwJAIAQNACADEPKBgIAACyAFIAZrIAFuDwsgACAHaiEAIAYgB2siBg0ACwsgAkEAIAEbIQACQCAEDQAgAxDygYCAAAsgAAuxAQEBfwJAAkAgAkEDSQ0AEOmBgIAAQRw2AgAMAQsCQCACQQFHDQAgACgCCCIDRQ0AIAEgAyAAKAIEa6x9IQELAkAgACgCFCAAKAIcRg0AIABBAEEAIAAoAiQRhYCAgACAgICAABogACgCFEUNAQsgAEEANgIcIABCADcDECAAIAEgAiAAKAIoEYaAgIAAgICAgABCAFMNACAAQgA3AgQgACAAKAIAQW9xNgIAQQAPC0F/C0gBAX8CQCAAKAJMQX9KDQAgACABIAIQhYKAgAAPCyAAEPGBgIAAIQMgACABIAIQhYKAgAAhAgJAIANFDQAgABDygYCAAAsgAgsPACAAIAGsIAIQhoKAgAALhgECAn8BfiAAKAIoIQFBASECAkAgAC0AAEGAAXFFDQBBAUECIAAoAhQgACgCHEYbIQILAkAgAEIAIAIgARGGgICAAICAgIAAIgNCAFMNAAJAAkAgACgCCCICRQ0AQQQhAQwBCyAAKAIcIgJFDQFBFCEBCyADIAAgAWooAgAgAmusfCEDCyADC0ICAX8BfgJAIAAoAkxBf0oNACAAEIiCgIAADwsgABDxgYCAACEBIAAQiIKAgAAhAgJAIAFFDQAgABDygYCAAAsgAgsrAQF+AkAgABCJgoCAACIBQoCAgIAIUw0AEOmBgIAAQT02AgBBfw8LIAGnC1wBAX8gACAAKAJIIgFBf2ogAXI2AkgCQCAAKAIAIgFBCHFFDQAgACABQSByNgIAQX8PCyAAQgA3AgQgACAAKAIsIgE2AhwgACABNgIUIAAgASAAKAIwajYCEEEAC+YBAQN/AkACQCACKAIQIgMNAEEAIQQgAhCLgoCAAA0BIAIoAhAhAwsCQCABIAMgAigCFCIEa00NACACIAAgASACKAIkEYWAgIAAgICAgAAPCwJAAkAgAigCUEEASA0AIAFFDQAgASEDAkADQCAAIANqIgVBf2otAABBCkYNASADQX9qIgNFDQIMAAsLIAIgACADIAIoAiQRhYCAgACAgICAACIEIANJDQIgASADayEBIAIoAhQhBAwBCyAAIQVBACEDCyAEIAUgARCCgoCAABogAiACKAIUIAFqNgIUIAMgAWohBAsgBAtnAQJ/IAIgAWwhBAJAAkAgAygCTEF/Sg0AIAAgBCADEIyCgIAAIQAMAQsgAxDxgYCAACEFIAAgBCADEIyCgIAAIQAgBUUNACADEPKBgIAACwJAIAAgBEcNACACQQAgARsPCyAAIAFuC0sBAX8jgICAgABBEGsiAySAgICAACAAIAEgAkH/AXEgA0EIahC0gICAABDbgoCAACECIAMpAwghASADQRBqJICAgIAAQn8gASACGwsEAEEACwIACwIACxQAQczRhIAAEJCCgIAAQdDRhIAACw4AQczRhIAAEJGCgIAACzQBAn8gABCSgoCAACIBKAIAIgI2AjgCQCACRQ0AIAIgADYCNAsgASAANgIAEJOCgIAAIAALswEBA38jgICAgABBEGsiAiSAgICAACACIAE6AA8CQAJAIAAoAhAiAw0AAkAgABCLgoCAAEUN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//8Aaq1CMIYQ9YKAgAAgACAEKQMINwMIIAAgBCkDADcDACAEQdAAaiSAgICAAAtLAgF+An8gAUL///////8/gyECAkACQCABQjCIp0H//wFxIgNB//8BRg0AQQQhBCADDQFBAkEDIAIgAIRQGw8LIAIgAIRQIQQLIAQL5wYEA38CfgF/AX4jgICAgABBgAFrIgUkgICAgAACQAJAAkAgAyAEQgBCABDrgoCAAEUNACADIAQQv4KAgABFDQAgAkIwiKciBkH//wFxIgdB//8BRw0BCyAFQRBqIAEgAiADIAQQ9YKAgAAgBSAFKQMQIgQgBSkDGCIDIAQgAxDtgoCAACAFKQMIIQIgBSkDACEEDAELAkAgASACQv///////////wCDIgggAyAEQv///////////wCDIgkQ64KAgABBAEoNAAJAIAEgCCADIAkQ64KAgABFDQAgASEEDAILIAVB8ABqIAEgAkIAQgAQ9YKAgAAgBSkDeCECIAUpA3AhBAwBCyAEQjCIp0H//wFxIQoCQAJAIAdFDQAgASEEDAELIAVB4ABqIAEgCEIAQoCAgICAgMC7wAAQ9YKAgAAgBSkDaCIIQjCIp0GIf2ohByAFKQNgIQQLAkAgCg0AIAVB0ABqIAMgCUIAQoCAgICAgMC7wAAQ9YKAgAAgBSkDWCIJQjCIp0GIf2ohCiAFKQNQIQMLIAlC////////P4NCgICAgICAwACEIQsgCEL///////8/g0KAgICAgIDAAIQhCAJAIAcgCkwNAANAAkACQCAIIAt9IAQgA1StfSIJQgBTDQACQCAJIAQgA30iBIRCAFINACAFQSBqIAEgAkIAQgAQ9YKAgAAgBSkDKCECIAUpAyAhBAwFCyAJQgGGIARCP4iEIQgMAQsgCEIBhiAEQj+IhCEICyAEQgGGIQQgB0F/aiIHIApKDQALIAohBwsCQAJAIAggC30gBCADVK19IglCAFkNACAIIQkMAQsgCSAEIAN9IgSEQgBSDQAgBUEwaiABIAJCAEIAEPWCgIAAIAUpAzghAiAFKQMwIQQMAQsCQCAJQv///////z9WDQADQCAEQj+IIQMgB0F/aiEHIARCAYYhBCADIAlCAYaEIglCgICAgICAwABUDQALCyAGQYCAAnEhCgJAIAdBAEoNACAFQcAAaiAEIAlC////////P4MgB0H4AGogCnKtQjCGhEIAQoCAgICAgMDDPxD1goCAACAFKQNIIQIgBSkDQCEEDAELIAlC////////P4MgByAKcq1CMIaEIQILIAAgBDcDACAAIAI3AwggBUGAAWokgICAgAALHAAgACACQv///////////wCDNwMIIAAgATcDAAvPCQQBfwF+BX8BfiOAgICAAEEwayIEJICAgIAAQgAhBQJAAkAgAkECSw0AIAJBAnQiAkGMwoSAAGooAgAhBiACQYDChIAAaigCACEHA0ACQAJAIAEoAgQiAiABKAJoRg0AIAEgAkEBajYCBCACLQAAIQIMAQsgARC8goCAACECCyACEMOCgIAADQALQQEhCAJAAkAgAkFVag4DAAEAAQtBf0EBIAJBLUYbIQgCQCABKAIEIgIgASgCaEYNACABIAJBAWo2AgQgAi0AACECDAELIAEQvIKAgAAhAgtBACEJAkACQAJAIAJBX3FByQBHDQADQCAJQQdGDQICQAJAIAEoAgQiAiABKAJoRg0AIAEgAkEBajYCBCACLQAAIQIMAQsgARC8goCAACECCyAJQYuAhIAAaiEKIAlBAWohCSACQSByIAosAABGDQALCwJAIAlBA0YNACAJQQhGDQEgA0UNAiAJQQRJDQIgCUEIRg0BCwJAIAEpA3AiBUIAUw0AIAEgASgCBEF/ajYCBAsgA0UNACAJQQRJDQAgBUIAUyECA0ACQCACDQAgASABKAIEQX9qNgIECyAJQX9qIglBA0sNAAsLIAQgCLJDAACAf5QQ74KAgAAgBCkDCCELIAQpAwAhBQwCCwJAAkACQAJAAkACQCAJDQBBACEJIAJBX3FBzgBHDQADQCAJQQJGDQICQAJAIAEoAgQiAiABKAJoRg0AIAEgAkEBajYCBCACLQAAIQIMAQsgARC8goCAACECCyAJQY+MhIAAaiEKIAlBAWohCSACQSByIAosAABGDQALCyAJDgQDAQEAAQsCQAJAIAEoAgQiAiABKAJoRg0AIAEgAkEBajYCBCACLQAAIQIMAQsgARC8goCAACECCwJAAkAgAkEoRw0AQQEhCQwBC0IAIQVCgICAgICA4P//ACELIAEpA3BCAFMNBiABIAEoAgRBf2o2AgQMBgsDQAJAAkAgASgCBCICIAEoAmhGDQAgASACQQFqNgIEIAItAAAhAgwBCyABELyC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IgxBCkcNAAwMCwsCQCAMQQpJDQBBASELDAsLA0AgBCAMQQJ0aigCAA0BQQEhCyAMQQFqIgxBCkYNCwwACwtBHCEWDAcLIAcgDDoAJ0EBIRQgCCENIAkhFiAXIREMAQsgCSEWCyAUIBYgDWsiASAUIAFKGyISIBBB/////wdzSg0DQT0hFiATIBAgEmoiDyATIA9KGyIMIA5KDQQgAEEgIAwgDyARENaCgIAAIAAgGCAQENCCgIAAIABBMCAMIA8gEUGAgARzENaCgIAAIABBMCASIAFBABDWgoCAACAAIA0gARDQgoCAACAAQSAgDCAPIBFBgMAAcxDWgoCAACAHKAI8IQEMAQsLC0EAIQsMAwtBPSEWCxDpgYCAACAWNgIAC0F/IQsLIAdBwABqJICAgIAAIAsLHAACQCAALQAAQSBxDQAgASACIAAQjIKAgAAaCwt7AQV/QQAhAQJAIAAoAgAiAiwAAEFQaiIDQQlNDQBBAA8LA0BBfyEEAkAgAUHMmbPmAEsNAEF/IAMgAUEKbCIBaiADIAFB/////wdzSxshBAsgACACQQFqIgM2AgAgAiwAASEFIAQhASADIQIgBUFQaiIDQQpJDQALIAQLvgQAAkACQAJAAkACQAJAAkACQAJAAkACQAJAAkACQAJAAkACQAJAAkAgAUF3ag4SAAECBQMEBgcICQoLDA0ODxAREgsgAiACKAIAIgFBBGo2AgAgACABKAIANgIADwsgAiACKAIAIgFBBGo2AgAgACABNAIANwMADwsgAiACKAIAIgFBBGo2AgAgACABNQIANwMADwsgAiACKAIAIgFBBGo2AgAgACABNAIANwMADwsgAiACKAIAIgFBBGo2AgAgACABNQIANwMADwsgAiACKAIAQQdqQXhxIgFBCGo2AgAgACABKQMANwMADwsgAiACKAIAIgFBBGo2AgAgACABMgEANwMADwsgAiACKAIAIgFBBGo2AgAgACABMwEANwMADwsgAiACKAIAIgFBBGo2AgAgACABMAAANwMADwsgAiACKAIAIgFBBGo2AgAgACABMQAANwMADwsgAiACKAIAQQdqQXhxIgFBCGo2AgAgACABKQMANwMADwsgAiACKAIAIgFBBGo2AgAgACABNQIANwMADwsgAiACKAIAQQdqQXhxIgFBCGo2AgAgACABKQMANwMADwsg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GAsgACAIQXhxIAFqIgFBAXI2AgQgACABaiABNgIAIAAgCUcNAUEAIAE2AvDahIAADwsgAiAIQX5xNgIEIAAgAUEBcjYCBCAAIAFqIAE2AgALAkAgAUH/AUsNACABQXhxQZDbhIAAaiEDAkACQEEAKALo2oSAACIFQQEgAUEDdnQiAXENAEEAIAUgAXI2AujahIAAIAMhAQwBCyADKAIIIgEgBEkNAwsgAyAANgIIIAEgADYCDCAAIAM2AgwgACABNgIIDwtBHyEDAkAgAUH///8HSw0AIAFBJiABQQh2ZyIDa3ZBAXEgA0EBdGtBPmohAwsgACADNgIcIABCADcCECADQQJ0QZjdhIAAaiEFAkACQAJAQQAoAuzahIAAIgZBASADdCICcQ0AQQAgBiACcjYC7NqEgAAgBSAANgIAIAAgBTYCGAwBCyABQQBBGSADQQF2ayADQR9GG3QhAyAFKAIAIQYDQCAGIgUoAgRBeHEgAUYNAiADQR12IQYgA0EBdCEDIAUgBkEEcWoiAigCECIGDQALIAJBEGoiASAESQ0DIAEgADYCACAAIAU2AhgLIAAgADYCDCAAIAA2AggPCyAFIARJDQEgBSgCCCIBIARJDQEgASAANgIMIAUgADYCCCAAQQA2AhggACAFNgIMIAAgATYCCAsPCxDfgoCAAAALawIBfwF+AkACQCAADQBBACECDAELIACtIAGtfiIDpyECIAEgAHJBgIAESQ0AQX8gAiADQiCIp0EARxshAgsCQCACEOCCgIAAIgBFDQAgAEF8ai0AAEEDcUUNACAAQQAgAhD4gYCAABoLIAALBwA/AEEQdAthAQJ/QQAoApzNhIAAIgEgAEEHakF4cSICaiEAAkACQAJAIAJFDQAgACABTQ0BCyAAEOeCgIAATQ0BIAAQtoCAgAANAQsQ6YGAgABBMDYCAEF/DwtBACAANgKczYSAACABC/oKBwF/AX4BfwJ+AX8BfgF/I4CAgIAAQfAAayIFJICAgIAAIARC////////////AIMhBgJAAkACQCABUCIHIAJC////////////AIMiCEKAgICAgIDAgIB/fEKAgICAgIDAgIB/VCAIUBsNACADQgBSIAZCgICAgICAwICAf3wiCUKAgICAgIDAgIB/ViAJQoCAgICAgMCAgH9RGw0BCwJAIAcgCEKAgICAgIDA//8AVCAIQoCAgICAgMD//wBRGw0AIAJCgICAgICAIIQhBCABIQMMAgsCQCADUCAGQoCAgICAgMD//wBUIAZCgICAgICAwP//AFEbDQAgBEKAgICAgIAghCEEDAILAkAgASAIQoCAgICAgMD//wCFhEIAUg0AQoCAgICAgOD//wAgAiADIAGFIAQgAoVCgICAgICAgICAf4WEUCIHGyEEQgAgASAHGyEDDAILIAMgBkKAgICAgIDA//8AhYRQDQECQCABIAiEQgBSDQAgAyAGhEIAUg0CIAMgAYMhAyAEIAKDIQQMAgsgAyAGhFBFDQAgASEDIAIhBAwBCyADIAEgAyABViAGIAhWIAYgCFEbIgobIQYgBCACIAobIglC////////P4MhCCACIAQgChsiC0IwiKdB//8BcSEMAkAgCUIwiKdB//8BcSIHDQAgBUHgAGogBiAIIAYgCCAIUCIHG3kgB0EGdK18pyIHQXFqEOqCgIAAQRAgB2shByAFKQNoIQggBSkDYCEGCyABIAMgChshAyALQv///////z+DIQECQCAMDQAgBUHQAGogAyABIAMgASABUCIKG3kgCkEGdK18pyIKQXFqEOqCgIAAQRAgCmshDCAFKQNYIQEgBSkDUCEDCyABQgOGIANCPYiEQoCAgICAgIAEhCEBIAhCA4YgBkI9iIQhCyADQgOGIQggBCAChSEDAkAgByAMRg0AAkAgByAMayIKQf8ATQ0AQgAhAUIBIQgMAQsgBUHAAGogCCABQYABIAprEOqCgIAAIAVBMGogCCABIAoQ9IKAgAAgBSkDMCAFKQNAIAUpA0iEQgBSrYQhCCAFKQM4IQELIAtCgICAgICAgASEIQsgBkIDhiEGAkACQCADQn9VDQBCACEDQgAhBCAGIAiFIAsgAYWEUA0CIAYgCH0hAiALIAF9IAYgCFStfSIEQv////////8DVg0BIAVBIGogAiAEIAIgBCAEUCIKG3kgCkEGdK18p0F0aiIKEOqCgIAAIAcgCmshByAFKQMoIQQgBSkDICECDAELIAEgC3wgCCAGfCICIAhUrXwiBEKAgICAgICACINQDQAgAkIBiCAEQj+GhCAIQgGDhCECIAdBAWohByAEQgGIIQQLIAlCgICAgICAgICAf4MhCAJAIAdB//8BSA0AIAhCgICAgICAwP//AIQhBEIAIQMMAQtBACEKAkACQCAHQQBMDQAgByEKDAELIAVBEGogAiAEIAdB/wBqEOqCgIAAIAUgAiAEQQEgB2sQ9IKAgAAgBSkDACAFKQMQIAUpAxiEQgBSrYQhAiAFKQMIIQQLIAJCA4ggBEI9hoQhAyAKrUIwhiAEQgOIQv///////z+DhCAIhCEEIAKnQQdxIQcCQAJAAkACQAJAEPKCgIAADgMAAQIDCwJAIAdBBEYNACAEIAMgB0EES618IgggA1StfCEEIAghAwwDCyAEIAMgA0IBg3wiCCADVK18IQQgCCEDDAMLIAQgAyAIQgBSIAdBAEdxrXwiCCADVK18IQQgCCEDDAELIAQgAyAIUCAHQQBHca18IgggA1StfCEEIAghAwsgB0UNAQsQ84KAgAAaCyAAIAM3AwAgACAENwMIIAVB8ABqJICAgIAAC1MBAX4CQAJAIANBwABxRQ0AIAEgA0FAaq2GIQJCACEBDAELIANFDQAgAUHAACADa62IIAIgA60iBIaEIQIgASAEhiEBCyAAIAE3AwAgACACNwMIC+YBAgF/An5BASEEAkAgAEIAUiABQv///////////wCDIgVCgICAgICAwP//AFYgBUKAgICAgIDA//8AURsNACACQgBSIANC////////////AIMiBkKAgICAgIDA//8AViAGQoCAgICAgMD//wBRGw0AAkAgAiAAhCAGIAWEhFBFDQBBAA8LAkAgAyABg0IAUw0AAkAgACACVCABIANTIAEgA1EbRQ0AQX8PCyAAIAKFIAEgA4WEQgBSDwsCQCAAIAJWIAEgA1UgASADURtFDQBBfw8LIAAgAoUgASADhYRCAFIhBAsgBAvYAQIBfwJ+QX8hBAJAIABCAFIgAUL///////////8AgyIFQoCAgICAgMD//wBWIAVCgICAgICAwP//AFEbDQAgAkIAUiADQv///////////wCDIgZCgICAgICAwP//AFYgBkKAgICAgIDA//8AURsNAAJAIAIgAIQgBiAFhIRQRQ0AQQAPCwJAIAMgAYNCAFMNACAAIAJUIAEgA1MgASADURsNASAAIAKFIAEgA4WEQgBSDwsgACACViABIANVIAEgA1EbDQAgACAChSABIAOFhEIAUiEECyAEC8EQBgF/A34DfwF+AX8LfiOAgICAAEHQAmsiBSSAgICAACAEQv///////z+DIQYgAkL///////8/gyEHIAQgAoVCgICAgICAgICAf4MhCCAEQjCIp0H//wFxIQkCQAJAAkAgAkIwiKdB//8BcSIKQYGAfmpBgoB+SQ0AQQAhCyAJQYGAfmpBgYB+Sw0BCwJAIAFQIAJC////////////AIMiDEKAgICAgIDA//8AVCAMQoCAgICAgMD//wBRGw0AIAJCgICAgICAIIQhCAwCCwJAIANQIARC////////////AIMiAkKAgICAgIDA//8AVCACQoCAgICAgMD//wBRGw0AIARCgICAgICAIIQhCCADIQEMAgsCQCABIAxCgICAgICAwP//AIWEQgBSDQACQCADIAJCgICAgICAwP//AIWEUEUNAEIAIQFCgICAgICA4P//ACEIDAMLIAhCgICAgICAwP//AIQhCEIAIQEMAgsCQCADIAJCgICAgICAwP//AIWEQgBSDQBCACEBDAILAkAgASAMhEIAUg0AQoCAgICAgOD//wAgCCADIAKEUBshCEIAIQEMAgsCQCADIAKEQgBSDQAgCEKAgICAgIDA//8AhCEIQgAhAQwCC0EAIQsCQCAMQv///////z9WDQAgBUHAAmogASAHIAEgByAHUCILG3kgC0EGdK18pyILQXFqEOqCgIAAQRAgC2shCyAFKQPIAiEHIAUpA8ACIQELIAJC////////P1YNACAFQbACaiADIAYgAyAGIAZQIg0beSANQQZ0rXynIg1BcWoQ6oKAgAAgDSALakFwaiELIAUpA7gCIQYgBSkDsAIhAwsgBUGgAmogA0IxiCAGQoCAgICAgMAAhCIOQg+GhCICQgBCgICAgLDmvIL1ACACfSIEQgAQ9oKAgAAgBUGQAmpCACAFKQOoAn1CACAEQgAQ9oKAgAAgBUGAAmogBSkDkAJCP4ggBSkDmAJCAYaEIgRCACACQgAQ9oKAgAAgBUHwAWogBEIAQgAgBSkDiAJ9QgAQ9oKAgAAgBUHgAWogBSkD8AFCP4ggBSkD+AFCAYaEIgRCACACQgAQ9oKAgAAgBUHQAWogBEIAQgAgBSkD6AF9QgAQ9oKAgAAgBUHAAWogBSkD0AFCP4ggBSkD2AFCAYaEIgRCACACQgAQ9oKAgAAgBUGwAWogBEIAQgAgBSkDyAF9QgAQ9oKAgAAgBUGgAWogAkIAIAUpA7AB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IAAgBDcDCCACQRBqJICAgIAAC4EBAgF/An4jgICAgABBEGsiAiSAgICAAAJAAkAgAQ0AQgAhA0IAIQQMAQsgAiABrUIAQfAAIAFnIgFBH3NrEOqCgIAAIAIpAwhCgICAgICAwACFQZ6AASABa61CMIZ8IQQgAikDACEDCyAAIAM3AwAgACAENwMIIAJBEGokgICAgAALBABBAAsEAEEAC1MBAX4CQAJAIANBwABxRQ0AIAIgA0FAaq2IIQFCACECDAELIANFDQAgAkHAACADa62GIAEgA60iBIiEIQEgAiAEiCECCyAAIAE3AwAgACACNwMIC6MLBgF/BH4DfwF+AX8KfiOAgICAAEHgAGsiBSSAgICAACAEQv///////z+DIQYgBCAChUKAgICAgICAgIB/gyEHIAJC////////P4MiCEIgiCEJIARCMIinQf//AXEhCgJAAkACQCACQjCIp0H//wFxIgtBgYB+akGCgH5JDQBBACEMIApBgYB+akGBgH5LDQELAkAgAVAgAkL///////////8AgyINQoCAgICAgMD//wBUIA1CgICAgICAwP//AFEbDQAgAkKAgICAgIAghCEHDAILAkAgA1AgBEL///////////8AgyICQoCAgICAgMD//wBUIAJCgICAgICAwP//AFEbDQAgBEKAgICAgIAghCEHIAMhAQwCCwJAIAEgDUKAgICAgIDA//8AhYRCAFINAAJAIAMgAoRQRQ0AQoCAgICAgOD//wAhB0IAIQEMAwsgB0KAgICAgIDA//8AhCEHQgAhAQwCCwJAIAMgAkKAgICAgIDA//8AhYRCAFINACABIA2EIQJCACEBAkAgAlBFDQBCgICAgICA4P//ACEHDAMLIAdCgICAgICAwP//AIQhBwwCCwJAIAEgDYRCAFINAEIAIQEMAgsCQCADIAKEQgBSDQBCACEBDAILQQAhDAJAIA1C////////P1YNACAFQdAAaiABIAggASAIIAhQIgwbeSAMQQZ0rXynIgxBcWoQ6oKAgABBECAMayEMIAUpA1giCEIgiCEJIAUpA1AhAQsgAkL///////8/Vg0AIAVBwABqIAMgBiADIAYgBlAiDht5IA5BBnStfKciDkFxahDqgoCAACAMIA5rQRBqIQwgBSkDSCEGIAUpA0AhAwsgA0IPhiINQoCA/v8PgyICIAFCIIgiBH4iDyANQiCIIg0gAUL/////D4MiAX58IhBCIIYiESACIAF+fCISIBFUrSACIAhC/////w+DIgh+IhMgDSAEfnwiESADQjGIIAZCD4YiFIRC/////w+DIgMgAX58IhUgEEIgiCAQIA9UrUIghoR8IhAgAiAJQoCABIQiBn4iFiANIAh+fCIJIBRCIIhCgICAgAiEIgIgAX58Ig8gAyAEfnwiFEIghnwiF3whASALIApqIAxqQYGAf2ohCgJAAkAgAiAEfiIYIA0gBn58IgQgGFStIAQgAyAIfnwiDSAEVK18IAIgBn58IA0gESATVK0gFSARVK18fCIEIA1UrXwgAyAGfiIDIAIgCH58IgIgA1StQiCGIAJCIIiEfCAEIAJCIIZ8IgIgBFStfCACIBRCIIggCSAWVK0gDyAJVK18IBQgD1StfEIghoR8IgQgAlStfCAEIBAgFVStIBcgEFStfHwiAiAEVK18IgRCgICAgICAwACDUA0AIApBAWohCgwBCyASQj+IIQMgBEIBhiACQj+IhCEEIAJCAYYgAUI/iIQhAiASQgGGIRIgAyABQgGGhCEBCwJAIApB//8BSA0AIAdCgICAgICAwP//AIQhB0IAIQEMAQsCQAJAIApBAEoNAAJAQQEgCmsiC0H/AEsNACAFQTBqIBIgASAKQf8AaiIKEOqCgIAAIAVBIGogAiAEIAoQ6oKAgAAgBUEQaiASIAEgCxD0goCAACAFIAIgBCALEPSCgIAAIAUpAyAgBSkDEIQgBSkDMCAFKQM4hEIAUq2EIRIgBSkDKCAFKQMYhCEBIAUpAwghBCAFKQMAIQIMAgtCACEBDAILIAqtQjCGIARC////////P4OEIQQLIAQgB4QhBwJAIBJQIAFCf1UgAUKAgICAgICAgIB/URsNACAHIAJCAXwiAVCtfCEHDAELAkAgEiABQoCAgICAgICAgH+FhEIAUQ0AIAIhAQwBCyAHIAIgAkIBg3wiASACVK18IQcLIAAgATcDACAAIAc3AwggBUHgAGokgICAgAALdQEBfiAAIAQgAX4gAiADfnwgA0IgiCICIAFCIIgiBH58IANC/////w+DIgMgAUL/////D4MiAX4iBUIgiCADIAR+fCIDQiCIfCADQv////8PgyACIAF+fCIBQiCIfDcDCCAAIAFCIIYgBUL/////D4OENwMAC1QBAX8jgICAgABBEGsiBSSAgICAACAFIAEgAiADIARCgICAgICAgICAf4UQ6YKAgAAgBSkDACEEIAAgBSkDCDcDCCAAIAQ3AwAgBUEQaiSAgICAAAubBAMBfwJ+BH8jgICAgABBIGsiAiSAgICAACABQv///////z+DIQMCQAJAIAFCMIhC//8BgyIEpyIFQf+Hf2pB/Q9LDQAgAEI8iCADQgSGhCEDIAVBgIh/aq0hBAJAAkAgAEL//////////w+DIgBCgYCAgICAgIAIVA0AIANCAXwhAwwBCyAAQoCAgICAgICACFINACADQgGDIAN8IQMLQgAgAyADQv////////8HViIFGyEAIAWtIAR8IQMMAQsCQCAAIAOEUA0AIARC//8BUg0AIABCPIggA0IEhoRCgICAgICAgASEIQBC/w8hAwwBCwJAIAVB/ocBTQ0AQv8PIQNCACEADAELAkBBgPgAQYH4ACAEUCIGGyIHIAVrIghB8ABMDQBCACEAQgAhAwwBCyACQRBqIAAgAyADQoCAgICAgMAAhCAGGyIDQYABIAhrEOqCgIAAIAIgACADIAgQ9IKAgAAgAikDACIDQjyIIAIpAwhCBIaEIQACQAJAIANC//////////8PgyAHIAVHIAIpAxAgAikDGIRCAFJxrYQiA0KBgICAgICAgAhUDQAgAEIBfCEADAELIANCgICAgICAgIAIUg0AIABCAYMgAHwhAAsgAEKAgICAgICACIUgACAAQv////////8HViIFGyEAIAWtIQMLIAJBIGokgICAgAAgA0I0hiABQoCAgICAgICAgH+DhCAAhL8LJwACQCAARQ0AQciFhIAAQbCJhIAAQRhBtZSEgAAQgICAgAAAC0EBCwIACwoAIAAkgICAgAALGgECfyOAgICAACAAa0FwcSIBJICAgIAAIAELCAAjgICAgAALIABBgICEgAAkgoCAgABBgICAgABBD2pBcHEkgYCAgAALDwAjgICAgAAjgYCAgABrCwgAI4KAgIAACwgAI4GAgIAACwuxTQIAQYCABAuARmludGVuc2l0eQBpbmZpbml0eQBCaW5kIGdyb3VwIGxpc3QgYXQgZnVsbCBjYXBhY2l0eQBTY2VuZSBtZXNoIGxpc3QgcmVhY2hlZCBmdWxsIGNhcGFjaXR5AENvdWxkbid0IHJlYWQgZW50aXJlIGZpbGUgaW50byBtZW1vcnkAQ291bGRuJ3QgYWxsb2NhdGUgbWVtb3J5AEtIUl9tYXRlcmlhbHNfYW5pc290cm9weQBtYXRyaXgAaW5kZXgAbWF4AC0rICAgMFgweAAtMFgrMFggMFgtMHgrMHggMHgAYnVmZmVyVmlldwB5Zm92AEtIUl90ZXh0dXJlX2Jhc2lzdQBvdXRwdXQAaW5wdXQAc3BvdABjb3VudABwb2ludABLSFJfbWF0ZXJpYWxzX3VubGl0AGNvcHlyaWdodABsaWdodABhc3NldABvZmZzZXQAYnl0ZU9mZnNldAB0YXJnZXQAS0hSX21hdGVyaWFsc19jbGVhcmNvYXQAYnVmZmVyVmlld3MAam9pbnRzAEtIUl9tYXRlcmlhbHNfdmFyaWFudHMAbGlnaHRzAHdlaWdodHMAdGFyZ2V0cwBLSFJfbWF0ZXJpYWxzX3BiclNwZWN1bGFyR2xvc3NpbmVzcwBwYnJNZXRhbGxpY1JvdWdobmVzcwBhY2Nlc3NvcnMAc2FtcGxlcnMAYnVmZmVycwBhbmltYXRpb25zAGV4dGVuc2lvbnMAc2tpbnMAY2hhbm5lbHMAbWF0ZXJpYWxzAG1hcHBpbmdzAHByaW1pdGl2ZXMAdmFsdWVzAGF0dHJpYnV0ZXMAdGV4dHVyZXMAc2NlbmVzAHRhcmdldE5hbWVzAG1lc2hlcwBpbWFnZXMAbm9kZXMAaW52ZXJzZUJpbmRNYXRyaWNlcwBpbmRpY2VzAGNhbnZhcwBleHRyYXMAY2FtZXJhcwBkZXNjcmlwdG9yID09IG51bGxwdHIAY2xlYXJjb2F0RmFjdG9yAHRoaWNrbmVzc0ZhY3RvcgBnbG9zc2luZXNzRmFjdG9yAHJvdWdobmVzc0ZhY3RvcgBjbGVhcmNvYXRSb3VnaG5lc3NGYWN0b3IAc2hlZW5Sb3VnaG5lc3NGYWN0b3IAc3BlY3VsYXJDb2xvckZhY3RvcgBkaWZmdXNlVHJhbnNtaXNzaW9uQ29sb3JGYWN0b3IAc2hlZW5Db2xvckZhY3RvcgBiYXNlQ29sb3JGYWN0b3IAc3BlY3VsYXJGYWN0b3IAdHJhbnNtaXNzaW9uRmFjdG9yAGRpZmZ1c2VUcmFuc21pc3Npb25GYWN0b3IAZW1pc3NpdmVGYWN0b3IAZGlmZnVzZUZhY3RvcgBpcmlkZXNjZW5jZUZhY3RvcgBtZXRhbGxpY0ZhY3RvcgBnZW5lcmF0b3IAY29sb3IAYXR0ZW51YXRpb25Db2xvcgBLSFJfbWF0ZXJpYWxzX2lvcgBpcmlkZXNjZW5jZUlvcgBmaWx0ZXIAbWluRmlsdGVyAG1hZ0ZpbHRlcgBzYW1wbGVyAGJ1ZmZlcgBTaGFkZXIAS0hSX21hdGVyaWFsc19zcGVjdWxhcgB6ZmFyAHpuZWFyAC9lbXNkay9lbXNjcmlwdGVuL3N5c3RlbS9saWIvd2ViZ3B1L3dlYmdwdS5jcHAARVhUX3RleHR1cmVfd2VicABhc3BlY3RSYXRpbwBza2VsZXRvbgByb3RhdGlvbgBhbmlzb3Ryb3B5Um90YXRpb24AdHJhbnNsYXRpb24AaW50ZXJwb2xhdGlvbgBLSFJfbWF0ZXJpYWxzX3RyYW5zbWlzc2lvbgBLSFJfbWF0ZXJpYWxzX2RpZmZ1c2VfdHJhbnNtaXNzaW9uAEVYVF9tZXNob3B0X2NvbXByZXNzaW9uAEtIUl9kcmFjb19tZXNoX2NvbXByZXNzaW9uAHZlcnNpb24AS0hSX21hdGVyaWFsc19kaXNwZXJzaW9uAG1pblZlcnNpb24AbWluAHNraW4AdnNfbWFpbgBmc19tYWluAGNoaWxkcmVuAEtIUl9tYXRlcmlhbHNfc2hlZW4AbmFuAGlyaWRlc2NlbmNlVGhpY2tuZXNzTWF4aW11bQBpcmlkZXNjZW5jZVRoaWNrbmVzc01pbmltdW0AS0hSX3RleHR1cmVfdHJhbnNmb3JtAC4vcnVudGltZS9hc3NldHMvc2hhZGVyL3NoYWRlci5yb3RhdGlvbi53Z3NsAC4vcnVudGltZS9hc3NldHMvc2hhZGVyL3NoYWRlci5ncmlkLndnc2wAS0hSX2xpZ2h0c19wdW5jdHVhbABkaXJlY3Rpb25hbABtYXRlcmlhbAB1cmkAS0hSX21hdGVyaWFsc19lbWlzc2l2ZV9zdHJlbmd0aABhbmlzb3Ryb3B5U3RyZW5ndGgAZW1pc3NpdmVTdHJlbmd0aABieXRlTGVuZ3RoAHBhdGgAbWVzaABFWFRfbWVzaF9ncHVfaW5zdGFuY2luZwB5bWFnAHhtYWcALi9yZXNvdXJjZXMvYXNzZXRzL2dsdGYvY3ViZS5nbHRmAGluZgBhbHBoYUN1dG9mZgBwZXJzcGVjdGl2ZQBTaGFkZXIgaGFzIG5vIGRldmljZSBvciBxdWV1ZQBNZXNoIGhhcyBubyBkZXZpY2Ugb3IgcXVldWUAc3BhcnNlAGFuaXNvdHJvcHlUZXh0dXJlAGNsZWFyY29hdFRleHR1cmUAdGhpY2tuZXNzVGV4dHVyZQBpcmlkZXNjZW5jZVRoaWNrbmVzc1RleHR1cmUAc3BlY3VsYXJHbG9zc2luZXNzVGV4dHVyZQBjbGVhcmNvYXRSb3VnaG5lc3NUZXh0dXJlAHNoZWVuUm91Z2huZXNzVGV4dHVyZQBtZXRhbGxpY1JvdWdobmVzc1RleHR1cmUAc3BlY3VsYXJDb2xvclRleHR1cmUAZGlmZnVzZVRyYW5zbWlzc2lvbkNvbG9yVGV4dHVyZQBzaGVlbkNvbG9yVGV4dHVyZQBiYXNlQ29sb3JUZXh0dXJlAHNwZWN1bGFyVGV4dHVyZQBvY2NsdXNpb25UZXh0dXJlAHRyYW5zbWlzc2lvblRleHR1cmUAZGlmZnVzZVRyYW5zbWlzc2lvblRleHR1cmUAbm9ybWFsVGV4dHVyZQBjbGVhcmNvYXROb3JtYWxUZXh0dXJlAGVtaXNzaXZlVGV4dHVyZQBkaWZmdXNlVGV4dHVyZQBpcmlkZXNjZW5jZVRleHR1cmUAdHlwZQBjb21wb25lbnRUeXBlAG1pbWVUeXBlAHNjZW5lAEtIUl9tYXRlcmlhbHNfdm9sdW1lAG5hbWUAb3V0ZXJDb25lQW5nbGUAaW5uZXJDb25lQW5nbGUAc2NhbGUAcmFuZ2UAbm9kZQBtb2RlAGFscGhhTW9kZQBieXRlU3RyaWRlAHNvdXJjZQBLSFJfbWF0ZXJpYWxzX2lyaWRlc2NlbmNlAHdncHVDcmVhdGVJbnN0YW5jZQBhdHRlbnVhdGlvbkRpc3RhbmNlAGN1YmUAdGV4Q29vcmQAZ3JpZABub3JtYWxpemVkAGV4dGVuc2lvbnNVc2VkAGV4dGVuc2lvbnNSZXF1aXJlZAB1bmRlZmluZWQAZG91YmxlU2lkZWQAb3J0aG9ncmFwaGljAHJiAHJ3YQBjYW1lcmEAd3JhcFQAVEFOR0VOVAB3cmFwUwBKT0lOVFMAV0VJR0hU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ZGV2aWNlAE91dCBvZiBtZW1vcnkAUmVzb3VyY2UgYnVzeQBJbnRlcnJ1cHRlZCBzeXN0ZW0gY2FsbABSZXNvdXJjZSB0ZW1wb3JhcmlseSB1bmF2YWlsYWJsZQBJbnZhbGlkIHNlZWsAQ3Jvc3MtZGV2aWNlIGxpbmsAUmVhZC1vbmx5IGZpbGUgc3lzdGVtAERpcmVjdG9yeSBub3QgZW1wdHkAQ29ubmVjdGlvbiByZXNldCBieSBwZWVyAE9wZXJhdGlvbiB0aW1lZCBvdXQAQ29ubmVjdGlvbiByZWZ1c2VkAEhvc3QgaXMgZG93bgBIb3N0IGlzIHVucmVhY2hhYmxlAEFkZHJlc3MgaW4gdXNlAEJyb2tlbiBwaXBlAEkvTyBlcnJvcgBObyBzdWNoIGRldmljZSBvciBhZGRyZXNzAEJsb2NrIGRldmljZSByZXF1aXJlZABObyBzdWNoIGRldmljZQBOb3QgYSBkaXJlY3RvcnkASXMgYSBkaXJlY3RvcnkAVGV4dCBmaWxlIGJ1c3kARXhlYyBmb3JtYXQgZXJyb3IASW52YWxpZCBhcmd1bWVudABBcmd1bWVudCBsaXN0IHRvbyBsb25nAFN5bWJvbGljIGxpbmsgbG9vcABGaWxlbmFtZSB0b28gbG9uZwBUb28gbWFueSBvcGVuIGZpbGVzIGluIHN5c3RlbQBObyBmaWxlIGRlc2NyaXB0b3JzIGF2YWlsYWJsZQBCYWQgZmlsZSBkZXNjcmlwdG9yAE5vIGNoaWxkIHByb2Nlc3MAQmFkIGFkZHJlc3MARmlsZSB0b28gbGFyZ2UAVG9vIG1hbnkgbGlua3MATm8gbG9ja3MgYXZhaWxhYmxlAFJlc291cmNlIGRlYWRsb2NrIHdvdWxkIG9jY3VyAFN0YXRlIG5vdCByZWNvdmVyYWJsZQBQcmV2aW91cyBvd25lciBkaWVkAE9wZXJhdGlvbiBjYW5jZWxlZABGdW5jdGlvbiBub3QgaW1wbGVtZW50ZWQATm8gbWVzc2FnZSBvZiBkZXNpcmVkIHR5cGUASWRlbnRpZmllciByZW1vdmVkAERldmljZSBub3QgYSBzdHJlYW0ATm8gZGF0YSBhdmFpbGFibGUARGV2aWNlIHRpbWVvdXQAT3V0IG9mIHN0cmVhbXMgcmVzb3VyY2VzAExpbmsgaGFzIGJlZW4gc2V2ZXJlZABQcm90b2NvbCBlcnJvcgBCYWQgbWVzc2FnZQBGaWxlIGRlc2NyaXB0b3IgaW4gYmFkIHN0YXRlAE5vdCBhIHNvY2tldABEZXN0aW5hdGlvbiBhZGRyZXNzIHJlcXVpcmVkAE1lc3NhZ2UgdG9vIGxhcmdlAFByb3RvY29sIHdyb25nIHR5cGUgZm9yIHNvY2tldABQcm90b2NvbCBub3QgYXZhaWxhYmxlAFByb3RvY29sIG5vdCBzdXBwb3J0ZWQAU29ja2V0IHR5cGUgbm90IHN1cHBvcnRlZABOb3Qgc3VwcG9ydGVkAFByb3RvY29sIGZhbWlseSBub3Qgc3VwcG9ydGVkAEFkZHJlc3MgZmFtaWx5IG5vdCBzdXBwb3J0ZWQgYnkgcHJvdG9jb2wAQWRkcmVzcyBub3QgYXZhaWxhYmxlAE5ldHdvcmsgaXMgZG93bgBOZXR3b3JrIHVucmVhY2hhYmxlAENvbm5lY3Rpb24gcmVzZXQgYnkgbmV0d29yawBDb25uZWN0aW9uIGFib3J0ZWQATm8gYnVmZmVyIHNwYWNlIGF2YWlsYWJsZQBTb2NrZXQgaXMgY29ubmVjdGVkAFNvY2tldCBub3QgY29ubmVjdGVkAENhbm5vdCBzZW5kIGFmdGVyIHNvY2tldCBzaHV0ZG93bgBPcGVyYXRpb24gYWxyZWFkeSBpbiBwcm9ncmVzcwBPcGVyYXRpb24gaW4gcHJvZ3Jlc3MAU3RhbGUgZmlsZSBoYW5kbGUAUmVtb3RlIEkvTyBlcnJvcgBRdW90YSBleGNlZWRlZABObyBtZWRpdW0gZm91bmQAV3JvbmcgbWVkaXVtIHR5cGUATXVsdGlob3AgYXR0ZW1wdGVkAFJlcXVpcmVkIGtleSBub3QgYXZhaWxhYmxlAEtleSBoYXMgZXhwaXJlZABLZXkgaGFzIGJlZW4gcmV2b2tlZABLZXkgd2FzIHJlamVjdGVkIGJ5IHNlcnZpY2UAAAAAAKUCWwDwAbUFjAUlAYMGHQOUBP8AxwMxAwsGvAGPAX8DygQrANoGrwBCA04D3AEOBBUAoQYNAZQCCwI4BmQCvAL/Al0D5wQLB88CywXvBdsF4QIeBkUChQCCAmwDbwTxAPMDGAXZANoDTAZUAnsBnQO9BAAAUQAVArsAswNtAP8BhQQvBfkEOABlAUYBnwC3BqgBcwJTAQAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAACEEAAAAAAAAAAAvAgAAAAAAAAAAAAAAAAAAAAAAAAAANQRHBFYEAAAAAAAAAAAAAAAAAAAAAKAEAAAAAAAAAAAAAAAAAAAAAAAARgVgBW4FYQYAAM8BAAAAAAAAAADJBukG+QYeBzkHSQdeBwAAAAAAAAAAAAAAANF0ngBXnb0qgHBSD///PicKAAAAZAAAAOgDAAAQJwAAoIYBAEBCDwCAlpgAAOH1BRgAAAA1AAAAcQAAAGv////O+///kr///wAAAAAAAAAAGQALABkZGQAAAAAFAAAAAAAACQAAAAALAAAAAAAAAAAZAAoKGRkZAwoHAAEACQsYAAAJBgsAAAsABhkAAAAZGRkAAAAAAAAAAAAAAAAAAAAADgAAAAAAAAAAGQALDRkZGQANAAACAAkOAAAACQAOAAAOAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAwAAAAAAAAAAAAAABMAAAAAEwAAAAAJDAAAAAAADAAADAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAQAAAAAAAAAAAAAAAPAAAABA8AAAAACRAAAAAAABAAABAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAEgAAAAAAAAAAAAAAEQAAAAARAAAAAAkSAAAAAAASAAASAAAaAAAAGhoaAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAABoAAAAaGhoAAAAAAAAJAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAUAAAAAAAAAAAAAAAXAAAAABcAAAAACRQAAAAAABQAABQAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAFgAAAAAAAAAAAAAAFQAAAAAVAAAAAAkWAAAAAAAWAAAWAAAwMTIzNDU2Nzg5QUJDREVGAEGAxgQLoAcAAAC/AAAAvwAAAD8AAAAAAAAAAAAAgD8AAIA/AAAAAAAAAAAAAAAAAAAAAAAAAD8AAAC/AAAAPwAAAAAAAAAAAACAPwAAAAAAAIA/AAAAAAAAgD8AAAAAAAAAPwAAAD8AAAA/AAAAAAAAAAAAAIA/AAAAAAAAAAAAAIA/AACAPwAAgD8AAAC/AAAAPwAAAD8AAAAAAAAAAAAAgD8AAIA/AACAPwAAAAAAAAAAAACAPwAAAL8AAAC/AAAAvwAAAAAAAAAAAACAvwAAgD8AAAAAAACAPwAAAAAAAAAAAAAAPwAAAL8AAAC/AAAAAAAAAAAAAIC/AAAAAAAAgD8AAIA/AACAPwAAAAAAAAA/AAAAPwAAAL8AAAAAAAAAAAAAgL8AAIA/AACAPwAAgD8AAIA/AACAPwAAAL8AAAA/AAAAvwAAAAAAAAAAAACAvwAAAD8AAAA/AAAAPwAAAAAAAIA/AAABAAIAAAACAAMABQAEAAcABQAHAAYABAAAAAMABAADAAcAAQAFAAYAAQAGAAIAAwACAAYAAwAGAAcABAAFAAEABAABAAAAAAAAAAAAAAAAAAC/AAAAAAAAAL8AAAAAAACAPwAAAAAAAIA/AAAAAAAAAAAAAAAAAAAAAAAAAD8AAAAAAAAAvwAAAAAAAIA/AAAAAAAAAAAAAIA/AAAAAAAAgD8AAAAAAAAAPwAAAAAAAAA/AAAAAAAAgD8AAAAAAAAAAAAAAAAAAIA/AACAPwAAgD8AAAC/AAAAAAAAAD8AAAAAAACAPwAAAAAAAIA/AACAPwAAAAAAAAAAAACAPwAAAQACAAAAAgADAAAAAAAFAAAAAAAAAAAAAAAPAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAANAAAADAAAAGApAQAAAAAAAAAAAAAAAAACAAAAAAAAAAAAAAAAAAAA//////////8AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAABwJQEAAAAAAAUAAAAAAAAAAAAAABAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA0AAAARAAAAaCkBAAAEAAAAAAAAAAAAAAEAAAAAAAAAAAAAAAAAAAD/////CgAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAgmAQBgLwEAAJQBD3RhcmdldF9mZWF0dXJlcwgrC2J1bGstbWVtb3J5Kw9idWxrLW1lbW9yeS1vcHQrFmNhbGwtaW5kaXJlY3Qtb3ZlcmxvbmcrCm11bHRpdmFsdWUrD211dGFibGUtZ2xvYmFscysTbm9udHJhcHBpbmctZnB0b2ludCsPcmVmZXJlbmNlLXR5cGVzKwhzaWduLWV4dA==';

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

