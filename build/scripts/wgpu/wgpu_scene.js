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
// include: /var/folders/hx/g36pxlqs1dj0kvmzszq_j3k00000gq/T/tmpofzphncm.js

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
Module['FS_createPath']("/", "shader", true, true);

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
    loadPackage({"files": [{"filename": "/shader/default.wgsl", "start": 0, "end": 1390}, {"filename": "/shader/grid.wgsl", "start": 1390, "end": 2529}, {"filename": "/shader/rotation.wgsl", "start": 2529, "end": 3416}], "remote_package_size": 3416});

  })();

// end include: /var/folders/hx/g36pxlqs1dj0kvmzszq_j3k00000gq/T/tmpofzphncm.js
// include: /var/folders/hx/g36pxlqs1dj0kvmzszq_j3k00000gq/T/tmpaa0pijv5.js

    // All the pre-js content up to here must remain later on, we need to run
    // it.
    if (Module['$ww'] || (typeof ENVIRONMENT_IS_PTHREAD != 'undefined' && ENVIRONMENT_IS_PTHREAD)) Module['preRun'] = [];
    var necessaryPreJSTasks = Module['preRun'].slice();
  // end include: /var/folders/hx/g36pxlqs1dj0kvmzszq_j3k00000gq/T/tmpaa0pijv5.js
// include: /var/folders/hx/g36pxlqs1dj0kvmzszq_j3k00000gq/T/tmppgr2zno0.js

    if (!Module['preRun']) throw 'Module.preRun should exist because file support used it; did a pre-js delete it?';
    necessaryPreJSTasks.forEach((task) => {
      if (Module['preRun'].indexOf(task) < 0) throw 'All preRun tasks that exist before user pre-js code should remain after; did you replace Module or modify Module.preRun?';
    });
  // end include: /var/folders/hx/g36pxlqs1dj0kvmzszq_j3k00000gq/T/tmppgr2zno0.js


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
var wasmBinaryFile = 'data:application/octet-stream;base64,AGFzbQEAAAAB6wEkYAJ/fwBgAX8Bf2ADf39/AX9gA39+fwF+YAZ/fH9/f38Bf2ABfwBgBX9/f39/AX9gAn9/AX9gBX9/fn9/AGAFf39/f38AYAV/f39+fgBgBn9/f39/fwBgAAF/YAN/fHwBf2ADf39/AGADf35/AX9gBH9/f38Bf2AEf35/fwF/YAAAYAR/f39/AGADf319AGACfH8Bf2ABfAF9YAJ9fwF/YAF9AX1gAXwBfGABfwF+YAJ8fwF8YAN8fH8BfGAHf39/f39/fwF/YAN+f38Bf2ACfn8Bf2ABfAF+YAJ/fgBgBH9+fn8AYAJ+fgF8AqwONwNlbnYZd2dwdVJlbmRlclBpcGVsaW5lUmVsZWFzZQAFA2VudgRleGl0AAUDZW52KWVtc2NyaXB0ZW5fc2V0X2tleWRvd25fY2FsbGJhY2tfb25fdGhyZWFkAAYDZW52J2Vtc2NyaXB0ZW5fc2V0X2tleXVwX2NhbGxiYWNrX29uX3RocmVhZAAGA2VuditlbXNjcmlwdGVuX3NldF9tb3VzZW1vdmVfY2FsbGJhY2tfb25fdGhyZWFkAAYDZW52J2Vtc2NyaXB0ZW5fc2V0X3doZWVsX2NhbGxiYWNrX29uX3RocmVhZAAGA2Vudh93Z3B1RGV2aWNlQ3JlYXRlQmluZEdyb3VwTGF5b3V0AAcDZW52HndncHVEZXZpY2VDcmVhdGVQaXBlbGluZUxheW91dAAHA2Vudh53Z3B1RGV2aWNlQ3JlYXRlUmVuZGVyUGlwZWxpbmUABwNlbnYkd2dwdVJlbmRlclBpcGVsaW5lR2V0QmluZEdyb3VwTGF5b3V0AAcDZW52GXdncHVEZXZpY2VDcmVhdGVCaW5kR3JvdXAABwNlbnYZd2dwdVBpcGVsaW5lTGF5b3V0UmVsZWFzZQAFA2Vudhp3Z3B1QmluZEdyb3VwTGF5b3V0UmVsZWFzZQAFA2VudiB3Z3B1UmVuZGVyUGFzc0VuY29kZXJTZXRQaXBlbGluZQAAA2VudhR3Z3B1UXVldWVXcml0ZUJ1ZmZlcgAIA2VudiF3Z3B1UmVuZGVyUGFzc0VuY29kZXJTZXRCaW5kR3JvdXAACQNlbnYXd2dwdVNoYWRlck1vZHVsZVJlbGVhc2UABQNlbnYkd2dwdVJlbmRlclBhc3NFbmNvZGVyU2V0VmVydGV4QnVmZmVyAAoDZW52I3dncHVSZW5kZXJQYXNzRW5jb2RlclNldEluZGV4QnVmZmVyAAoDZW52IHdncHVSZW5kZXJQYXNzRW5jb2RlckRyYXdJbmRleGVkAAsDZW52HHdncHVEZXZpY2VDcmVhdGVTaGFkZXJNb2R1bGUABwNlbnYWd2dwdURldmljZUNyZWF0ZUJ1ZmZlcgAHA2VudhxlbXNjcmlwdGVuX3dlYmdwdV9nZXRfZGV2aWNlAAwDZW52EndncHVEZXZpY2VHZXRRdWV1ZQABA2Vudh5lbXNjcmlwdGVuX3JlcXVlc3RfcG9pbnRlcmxvY2sABwNlbnYoZW1zY3JpcHRlbl9zZXRfcmVzaXplX2NhbGxiYWNrX29uX3RocmVhZAAGA2Vudh9lbXNjcmlwdGVuX2dldF9lbGVtZW50X2Nzc19zaXplAAIDZW52H2Vtc2NyaXB0ZW5fc2V0X2VsZW1lbnRfY3NzX3NpemUADQNlbnYUd2dwdVN3YXBDaGFpblJlbGVhc2UABQNlbnYQd2dwdVF1ZXVlUmVsZWFzZQAFA2VudhF3Z3B1RGV2aWNlUmVsZWFzZQAFA2VudiJ3Z3B1U3dhcENoYWluR2V0Q3VycmVudFRleHR1cmVWaWV3AAEDZW52HndncHVEZXZpY2VDcmVhdGVDb21tYW5kRW5jb2RlcgAHA2VudiF3Z3B1Q29tbWFuZEVuY29kZXJCZWdpblJlbmRlclBhc3MABwNlbnYYd2dwdVJlbmRlclBhc3NFbmNvZGVyRW5kAAUDZW52GHdncHVDb21tYW5kRW5jb2RlckZpbmlzaAAHA2Vudg93Z3B1UXVldWVTdWJtaXQADgNlbnYcd2dwdVJlbmRlclBhc3NFbmNvZGVyUmVsZWFzZQAFA2Vudhl3Z3B1Q29tbWFuZEVuY29kZXJSZWxlYXNlAAUDZW52GHdncHVDb21tYW5kQnVmZmVyUmVsZWFzZQAFA2VudhZ3Z3B1VGV4dHVyZVZpZXdSZWxlYXNlAAUDZW52GGVtc2NyaXB0ZW5fc2V0X21haW5fbG9vcAAOA2Vudhl3Z3B1SW5zdGFuY2VDcmVhdGVTdXJmYWNlAAcDZW52GXdncHVEZXZpY2VDcmVhdGVTd2FwQ2hhaW4AAhZ3YXNpX3NuYXBzaG90X3ByZXZpZXcxDmNsb2NrX3RpbWVfZ2V0AA8DZW52EF9fc3lzY2FsbF9vcGVuYXQAEANlbnYRX19zeXNjYWxsX2ZjbnRsNjQAAgNlbnYPX19zeXNjYWxsX2lvY3RsAAIWd2FzaV9zbmFwc2hvdF9wcmV2aWV3MQhmZF93cml0ZQAQFndhc2lfc25hcHNob3RfcHJldmlldzEHZmRfcmVhZAAQFndhc2lfc25hcHNob3RfcHJldmlldzEIZmRfY2xvc2UAARZ3YXNpX3NuYXBzaG90X3ByZXZpZXcxB2ZkX3NlZWsAEQNlbnYJX2Fib3J0X2pzABIDZW52FmVtc2NyaXB0ZW5fcmVzaXplX2hlYXAAAQNlbnYNX19hc3NlcnRfZmFpbAATA6kBpwESBQAADgAAEgICAgIBAAUFEwAFAAUABQUFFA4AAAAFAAAAAAAAExMAAgEFAAUFEAEFAAUSEgcSDAwHBhUWFhcYAQUFAQEZAQIDAgIBAQcHBwICARAPDwIaGgEBAhADBwUFDBIBBwcHBwEBBQUHDAwMEgUbGAEDBwcHAQEBHBkCBxsGHQ4BEx4fHwkCBAAgASECBxIBAgUHBwAMASIiIwEFBQEMEgwMDAQFAXABEBAFBgEBggKCAgYSA38BQYCABAt/AUEAC38BQQALB7MCDgZtZW1vcnkCABFfX3dhc21fY2FsbF9jdG9ycwA3Bm1hbGxvYwDKARlfX2luZGlyZWN0X2Z1bmN0aW9uX3RhYmxlAQAQX19tYWluX2FyZ2NfYXJndgBsBmZmbHVzaAB7CHN0cmVycm9yALABFWVtc2NyaXB0ZW5fc3RhY2tfaW5pdADaARllbXNjcmlwdGVuX3N0YWNrX2dldF9mcmVlANsBGWVtc2NyaXB0ZW5fc3RhY2tfZ2V0X2Jhc2UA3AEYZW1zY3JpcHRlbl9zdGFja19nZXRfZW5kAN0BGV9lbXNjcmlwdGVuX3N0YWNrX3Jlc3RvcmUA1wEXX2Vtc2NyaXB0ZW5fc3RhY2tfYWxsb2MA2AEcZW1zY3JpcHRlbl9zdGFja19nZXRfY3VycmVudADZAQkcAQBBAQsPP0BBQlNla3+AAYEBgwGrAawBwgHDAQrmgAWnAQgAENoBEKcBC3ABDH9BgICEgAAhASAAIAE2AgBBGCECIAAgAjsBBEEGIQMgACADaiEEQQAhBSAEIAU7AQBB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gIAAIaUBIAMgpQE4AoQBIAMoAqABIaYBIAMgpgE2AuQDQZABIacBIAMgpwFqIagBIKgBIakBIAMgqQE2AuADIAMoAuQDIaoBIAMoAuADIasBIAMgqgE2AvADIAMgqwE2AuwDIAMoAvADIawBIAMgrAE2AoQEIAMoAoQEIa0BIAMgrQE2AogEIAMoAogEIa4BIAMoAogEIa8BIAMgrgE2ApAEIAMgrwE2AowEIAMoApAEIbABILABKgIAIbEBIAMoAowEIbIBILIBKgIAIbMBIAMoApAEIbQBILQBKgIEIbUBIAMoAowEIbYBILYBKgIEIbcBILUBILcBlCG4ASCxASCzAZQhuQEguQEguAGSIboBIAMoApAEIbsBILsBKgIIIbwBIAMoAowEIb0BIL0BKgIIIb4BILwBIL4BlCG/ASC/ASC6AZIhwAEgwAGRIcEBIAMgwQE4AugDIAMqAugDIcIBQwAAADQhwwEgwgEgwwFdIcQBQQEhxQEgxAEgxQFxIcYBAkACQCDGAUUNACADKALsAyHHASADIMcBNgL0AyADKAL0AyHIAUEAIckBIMkBsiHKASDIASDKATgCCCADKAL0AyHLAUEAIcwBIMwBsiHNASDLASDNATgCBCADKAL0AyHOAUEAIc8BIM8BsiHQASDOASDQATgCAAwBCyADKALwAyHRASADKgLoAyHSAUMAAIA/IdMBINMBINIBlSHUASADKALsAyHVASADINEBNgKABCADINQBOAL8AyADINUBNgL4AyADKAKABCHWASDWASoCACHXASADKgL8AyHYASDXASDYAZQh2QEgAygC+AMh2gEg2gEg2QE4AgAgAygCgAQh2wEg2wEqAgQh3AEgAyoC/AMh3QEg3AEg3QGUId4BIAMoAvgDId8BIN8BIN4BOAIEIAMoAoAEIeABIOABKgIIIeEBIAMqAvwDIeIBIOEBIOIBlCHjASADKAL4AyHkASDkASDjATgCCAsgAyoChAEh5QEgAyoCkAEh5gEg5QEg5gGUIecBIAMoAqgBIegBIOgBIOcBOAIAIAMqAoQBIekBIAMqApQBIeoBIOkBIOoBlCHrASADKAKoASHsASDsASDrATgCBCADKgKEASHtASADKgKYASHuASDtASDuAZQh7wEgAygCqAEh8AEg8AEg7wE4AgggAyoCiAEh8QEgAygCqAEh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OgAPAkACQCAAKAIQIgMNAAJAIAAQkYGAgABFDQBBfyEDDAILIAAoAhAhAwsCQCAAKAIUIgQgA0YNACAAKAJQIAFB/wFxIgNGDQAgACAEQQFqNgIUIAQgAToAAAwBCwJAIAAgAkEPakEBIAAoAiQRgoCAgACAgICAAEEBRg0AQX8hAwwBCyACLQAPIQMLIAJBEGokgICAgAAgAwsMACAAIAEQnYGAgAALewECfwJAAkAgASgCTCICQQBIDQAgAkUNASACQf////8DcRCmgYCAACgCGEcNAQsCQCAAQf8BcSICIAEoAlBGDQAgASgCFCIDIAEoAhBGDQAgASADQQFqNgIUIAMgADoAACACDwsgASACEJuBgIAADwsgACABEJ6BgIAAC4QBAQN/AkAgAUHMAGoiAhCfgYCAAEUNACABEPeAgIAAGgsCQAJAIABB/wFxIgMgASgCUEYNACABKAIUIgQgASgCEEYNACABIARBAWo2AhQgBCAAOgAADAELIAEgAxCbgYCAACEDCwJAIAIQoIGAgABBgICAgARxRQ0AIAIQoYGAgAALIAMLGwEBfyAAIAAoAgAiAUH/////AyABGzYCACABCxQBAX8gACgCACEBIABBADYCACABCw0AIABBARCVgYCAABoL7AEBBH8Q74CAgAAoAgAQsIGAgAAhAQJAAkBBACgC7LCEgABBAE4NAEEBIQIMAQtBoLCEgAAQ94CAgABFIQILQQAoAuiwhIAAIQNBACgCqLGEgAAhBAJAIABFDQAgAC0AAEUNACAAIAAQsYGAgABBAUGgsISAABCTgYCAABpBOkGgsISAABCcgYCAABpBIEGgsISAABCcgYCAABoLIAEgARCxgYCAAEEBQaCwhIAAEJOBgIAAGkEKQaCwhIAAEJyBgIAAGkEAIAQ2AqixhIAAQQAgAzYC6LCEgAACQCACDQBBoLCEgAAQ+ICAgAALCzsBAX8jgICAgABBEGsiAiSAgICAACACIAE2AgxBuLGEgAAgACABEMGBgIAAIQEgAkEQaiSAgICAACABCwQAQSoLCAAQpIGAgAALCABBhLeEgAALIABBAEHktoSAADYC5LeEgABBABClgYCAADYCnLeEgAALYAEBfwJAAkAgACgCTEEASA0AIAAQ94CAgAAhASAAQgBBABCLgYCAABogACAAKAIAQV9xNgIAIAFF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ERILIAIgAigCACIBQQRqNgIAIAAgASgCADYCAA8LIAIgAigCACIBQQRqNgIAIAAgATQCADcDAA8LIAIgAigCACIBQQRqNgIAIAAgATUCADcDAA8LIAIgAigCACIBQQRqNgIAIAAgATQCADcDAA8LIAIgAigCACIBQQRqNgIAIAAgATUCADcDAA8LIAIgAigCAEEHakF4cSIBQQhqNgIAIAAgASkDADcDAA8LIAIgAigCACIBQQRqNgIAIAAgATIBADcDAA8LIAIgAigCACIBQQRqNgIAIAAgATMBADcDAA8LIAIgAigCACIBQQRqNgIAIAAgATAAADcDAA8LIAIgAigCACIBQQRqNgIAIAAgATEAADcDAA8LIAIgAigCAEEHakF4cSIBQQhqNgIAIAAgASkDADcDAA8LIAIgAigCACIBQQRqNgIAIAAgATUCADcDAA8LIAIgAigCAEEHakF4cSIBQQhqNgIAIAAgASkDADcDAA8LIAIgAigCAEEHakF4cSIBQQhqNgIAIAAgASkDADcDAA8LIAIgAigCACIBQQRqNgIAIAAgATQCADcDAA8LIAIgAigCACIBQQRqNgIAIAAgATUCADcDAA8LIAIgAigCAEEHakF4cSIBQQhqNgIAIAAgASsDADkDAA8LIAAgAiADEYCAgIAAgICAgAALC0ABAX8CQCAAUA0AA0AgAUF/aiIBIACnQQ9xQYCwhIAAai0AACACcjoAACAAQg9WIQMgAEIEiCEAIAMNAAsLIAELNgEBfwJAIABQDQADQCABQX9qIgEgAKdBB3FBMHI6AAAgAEIHViECIABCA4ghACACDQALCyABC4oBAgF+A38CQAJAIABCgICAgBBaDQAgACECDAELA0AgAUF/aiIBIAAgAEIKgCICQgp+fadBMHI6AAAgAEL/////nwFWIQMgAiEAIAMNAAsLAkAgAlANACACpyEDA0AgAUF/aiIBIAMgA0EKbiIEQQpsa0EwcjoAACADQQlLIQUgBCEDIAUNAAsLIAELhAEBAX8jgICAgABBgAJrIgUkgICAgAACQCACIANMDQAgBEGAwARxDQAgBSABIAIgA2siA0GAAiADQYACSSICGxD+gICAABoCQCACDQADQCAAIAVBgAIQuoGAgAAgA0GAfmoiA0H/AUsNAAsLIAAgBSADELqBgIAACyAFQYACaiSAgICAAAsaACAAIAEgAkGOgICAAEGPgICAABC4gYCAAAvIGQYCfwF+DH8CfgR/AXwjgICAgABBsARrIgYkgICAgABBACEHIAZBADYCLAJAAkAgARDEgYCAACIIQn9VDQBBASEJQfCBhIAAIQogAZoiARDEgYCAACEIDAELAkAgBEGAEHFFDQBBASEJQfOBhIAAIQoMAQtB9oGEgABB8YGEgAAgBEEBcSIJGyEKIAlFIQcLAkACQCAIQoCAgICAgID4/wCDQoCAgICAgID4/wBSDQAgAEEgIAIgCUEDaiILIARB//97cRDAgYCAACAAIAogCRC6gYCAACAAQeaChIAAQeSDhIAAIAVBIHEiDBtB/YKEgABB6IOEgAAgDBsgASABYhtBAxC6gYCAACAAQSAgAiALIARBgMAAcxDAgYCAACACIAsgAiALShshDQwBCyAGQRBqIQ4CQAJAAkACQCABIAZBLGoQt4GAgAAiASABoCIBRAAAAAAAAAAAYQ0AIAYgBigCLCILQX9qNgIsIAVBIHIiD0HhAEcNAQwDCyAFQSByIg9B4QBGDQJBBiADIANBAEgbIRAgBigCLCERDAELIAYgC0FjaiIRNgIsQQYgAyADQQBIGyEQIAFEAAAAAAAAsEGiIQELIAZBMGpBAEGgAiARQQBIG2oiEiEMA0AgDCAB/AMiCzYCACAMQQRqIQwgASALuKFEAAAAAGXNzUGiIgFEAAAAAAAAAABiDQALAkACQCARQQFODQAgESETIAwhCyASIRQMAQsgEiEUIBEhEwNAIBNBHSATQR1JGyETAkAgDEF8aiILIBRJDQAgE60hFUIAIQgDQCALIAs1AgAgFYYgCEL/////D4N8IhYgFkKAlOvcA4AiCEKAlOvcA359PgIAIAtBfGoiCyAUTw0ACyAWQoCU69wDVA0AIBRBfGoiFCAIPgIACwJAA0AgDCILIBRNDQEgC0F8aiIMKAIARQ0ACwsgBiAGKAIsIBNrIhM2AiwgCyEMIBNBAEoNAAsLAkAgE0F/Sg0AIBBBGWpBCW5BAWohFyAPQeYARiEYA0BBACATayIMQQkgDEEJSRshDQJAAkAgFCALSQ0AIBQoAgBFQQJ0IQwMAQtBgJTr3AMgDXYhGUF/IA10QX9zIRpBACETIBQhDANAIAwgDCgCACIDIA12IBNqNgIAIAMgGnEgGWwhEyAMQQRqIgwgC0kNAAsgFCgCAEVBAnQhDCATRQ0AIAsgEzYCACALQQRqIQsLIAYgBigCLCANaiITNgIsIBIgFCAMaiIUIBgbIgwgF0ECdGogCyALIAxrQQJ1IBdKGyELIBNBAEgNAAsLQQAhEwJAIBQgC08NACASIBRrQQJ1QQlsIRNBCiEMIBQoAgAiA0EKSQ0AA0AgE0EBaiETIAMgDEEKbCIMTw0ACwsCQCAQQQAgEyAPQeYARhtrIBBBAEcgD0HnAEZxayIMIAsgEmtBAnVBCWxBd2pODQAgBkEwakGEYEGkYiARQQBIG2ogDEGAyABqIgNBCW0iGUECdGohDUEKIQwCQCADIBlBCWxrIgNBB0oNAANAIAxBCmwhDCADQQFqIgNBCEcNAAsLIA1BBGohGgJAAkAgDSgCACIDIAMgDG4iFyAMbGsiGQ0AIBogC0YNAQsCQAJAIBdBAXENAEQAAAAAAABAQyEBIAxBgJTr3ANHDQEgDSAUTQ0BIA1BfGotAABBAXFFDQELRAEAAAAAAEBDIQELRAAAAAAAAOA/RAAAAAAAAPA/RAAAAAAAAPg/IBogC0YbRAAAAAAAAPg/IBkgDEEBdiIaRhsgGSAaSRshGwJAIAcNACAKLQAAQS1HDQAgG5ohGyABmiEBCyANIAMgGWsiAzYCACABIBugIAFhDQAgDSADIAxqIgw2AgACQCAMQYCU69wDSQ0AA0AgDUEANgIAAkAgDUF8aiINIBRPDQAgFEF8aiIUQQA2AgALIA0gDSgCAEEBaiIMNgIAIAxB/5Pr3ANLDQALCyASIBRrQQJ1QQlsIRNBCiEMIBQoAgAiA0EKSQ0AA0AgE0EBaiETIAMgDEEKbCIMTw0ACwsgDUEEaiIMIAsgCyAMSxshCwsCQANAIAsiDCAUTSIDDQEgDEF8aiILKAIARQ0ACwsCQAJAIA9B5wBGDQAgBEEIcSEZDAELIBNBf3NBfyAQQQEgEBsiCyATSiATQXtKcSINGyALaiEQQX9BfiANGyAFaiEFIARBCHEiGQ0AQXchCwJAIAMNACAMQXxqKAIAIg1FDQBBCiEDQQAhCyANQQpwDQADQCALIhlBAWohCyANIANBCmwiA3BFDQALIBlBf3MhCwsgDCASa0ECdUEJbCEDAkAgBUFfcUHGAEcNAEEAIRkgECADIAtqQXdqIgtBACALQQBKGyILIBAgC0gbIRAMAQtBACEZIBAgEyADaiALakF3aiILQQAgC0EAShsiCyAQIAtIGyEQC0F/IQ0gEEH9////B0H+////ByAQIBlyIhobSg0BIBAgGkEAR2pBAWohAwJAAkAgBUFfcSIYQcYARw0AIBMgA0H/////B3NKDQMgE0EAIBNBAEobIQsMAQsCQCAOIBMgE0EfdSILcyALa60gDhC/gYCAACILa0EBSg0AA0AgC0F/aiILQTA6AAAgDiALa0ECSA0ACwsgC0F+aiIXIAU6AABBfyENIAtBf2pBLUErIBNBAEgbOgAAIA4gF2siCyADQf////8Hc0oNAgtBfyENIAsgA2oiCyAJQf////8Hc0oNASAAQSAgAiALIAlqIgUgBBDAgYCAACAAIAogCRC6gYCAACAAQTAgAiAFIARBgIAEcxDAgYCAAAJAAkACQAJAIBhBxgBHDQAgBkEQakEJciETIBIgFCAUIBJLGyIDIRQDQCAUNQIAIBMQv4GAgAAhCwJAAkAgFCADRg0AIAsgBkEQak0NAQNAIAtBf2oiC0EwOgAAIAsgBkEQaksNAAwCCwsgCyATRw0AIAtBf2oiC0EwOgAACyAAIAsgEyALaxC6gYCAACAUQQRqIhQgEk0NAAsCQCAaRQ0AIABB7IOEgABBARC6gYCAAAsgFCAMTw0BIBBBAUgNAQNAAkAgFDUCACATEL+BgIAAIgsgBkEQak0NAANAIAtBf2oiC0EwOgAAIAsgBkEQaksNAAsLIAAgCyAQQQkgEEEJSBsQuoGAgAAgEEF3aiELIBRBBGoiFCAMTw0DIBBBCUohAyALIRAgAw0ADAMLCwJAIBBBAEgNACAMIBRBBGogDCAUSxshDSAGQRBqQQlyIRMgFCEMA0ACQCAMNQIAIBMQv4GAgAAiCyATRw0AIAtBf2oiC0EwOgAACwJAAkAgDCAURg0AIAsgBkEQak0NAQNAIAtBf2oiC0EwOgAAIAsgBkEQaksNAAwCCwsgACALQQEQuoGAgAAgC0EBaiELIBAgGXJFDQAgAEHsg4SAAEEBELqBgIAACyAAIAsgEyALayIDIBAgECADShsQuoGAgAAgECADayEQIAxBBGoiDCANTw0BIBBBf0oNAAsLIABBMCAQQRJqQRJBABDAgYCAACAAIBcgDiAXaxC6gYCAAAwCCyAQIQsLIABBMCALQQlqQQlBABDAgYCAAAsgAEEgIAIgBSAEQYDAAHMQwIGAgAAgAiAFIAIgBUobIQ0MAQsgCiAFQRp0QR91QQlxaiEXAkAgA0ELSw0AQQwgA2shC0QAAAAAAAAwQCEbA0AgG0QAAAAAAAAwQKIhGyALQX9qIgsNAAsCQCAXLQAAQS1HDQAgGyABmiAboaCaIQEMAQsgASAboCAboSEBCwJAIAYoAiwiDCAMQR91IgtzIAtrrSAOEL+BgIAAIgsgDkcNACALQX9qIgtBMDoAACAGKAIsIQwLIAlBAnIhGSAFQSBxIRQgC0F+aiIaIAVBD2o6AAAgC0F/akEtQSsgDEEASBs6AAAgA0EBSCAEQQhxRXEhEyAGQRBqIQwDQCAMIgsgAfwCIgxBgLCEgABqLQAAIBRyOgAAIAEgDLehRAAAAAAAADBAoiEBAkAgC0EBaiIMIAZBEGprQQFHDQAgAUQAAAAAAAAAAGEgE3ENACALQS46AAEgC0ECaiEMCyABRAAAAAAAAAAAYg0AC0F/IQ0gA0H9////ByAZIA4gGmsiFGoiE2tKDQAgAEEgIAIgEyADQQJqIAwgBkEQamsiCyALQX5qIANIGyALIAMbIgNqIgwgBBDAgYCAACAAIBcgGRC6gYCAACAAQTAgAiAMIARBgIAEcxDAgYCAACAAIAZBEGogCxC6gYCAACAAQTAgAyALa0EAQQAQwIGAgAAgACAaIBQQuoGAgAAgAEEgIAIgDCAEQYDAAHMQwIGAgAAgAiAMIAIgDEobIQ0LIAZBsARqJICAgIAAIA0LLgEBfyABIAEoAgBBB2pBeHEiAkEQajYCACAAIAIpAwAgAikDCBDUgYCAADkDAAsFACAAvQsZAAJAIAANAEEADwsQ74CAgAAgADYCAEF/CywBAX4gAEEANgIMIAAgAUKAlOvcA4AiAjcDACAAIAEgAkKAlOvcA359PgIIC6wCAQF/QQEhAwJAAkAgAEUNACABQf8ATQ0BAkACQBCmgYCAACgCYCgCAA0AIAFBgH9xQYC/A0YNAxDvgICAAEEZNgIADAELAkAgAUH/D0sNACAAIAFBP3FBgAFyOgABIAAgAUEGdkHAAXI6AABBAg8LAkACQCABQYCwA0kNACABQYBAcUGAwANHDQELIAAgAUE/cUGAAXI6AAIgACABQQx2QeABcjoAACAAIAFBBnZBP3FBgAFyOgABQQMPCwJAIAFBgIB8akH//z9LDQAgACABQT9xQYABcjoAAyAAIAFBEnZB8AFyOgAAIAAgAUEGdkE/cUGAAXI6AAIgACABQQx2QT9xQYABcjoAAUEEDwsQ74CAgABBGTYCAAtBfyEDCyADDwsgACABOgAAQQELGAACQCAADQBBAA8LIAAgAUEAEMeBgIAACwkAELSAgIAAAAuQJwEMfyOAgICAAEEQayIBJICAgIAAAkACQAJAAkACQCAAQfQBSw0AAkBBACgCmMCEgAAiAkEQIABBC2pB+ANxIABBC0kbIgNBA3YiBHYiAEEDcUUNAAJAAkAgAEF/c0EBcSAEaiIDQQN0IgBBwMCEgABqIgUgAEHIwISAAGooAgAiBCgCCCIARw0AQQAgAkF+IAN3cTYCmMCEgAAMAQsgAEEAKAKowISAAEkNBCAAKAIMIARHDQQgACAFNgIMIAUgADYCCAsgBEEIaiEAIAQgA0EDdCIDQQNyNgIEIAQgA2oiBCAEKAIEQQFyNgIEDAULIANBACgCoMCEgAAiBk0NAQJAIABFDQACQAJAIAAgBHRBAiAEdCIAQQAgAGtycWgiBUEDdCIAQcDAhIAAaiIHIABByMCEgABqKAIAIgAoAggiBEcNAEEAIAJBfiAFd3EiAjYCmMCEgAAMAQsgBEEAKAKowISAAEkNBCAEKAIMIABHDQQgBCAHNgIMIAcgBDYCCAsgACADQQNyNgIEIAAgA2oiByAFQQN0IgQgA2siA0EBcjYCBCAAIARqIAM2AgACQCAGRQ0AIAZBeHFBwMCEgABqIQVBACgCrMCEgAAhBAJAAkAgAkEBIAZBA3Z0IghxDQBBACACIAhyNgKYwISAACAFIQgMAQsgBSgCCCIIQQAoAqjAhIAASQ0FCyAFIAQ2AgggCCAENgIMIAQgBTYCDCAEIAg2AggLIABBCGohAEEAIAc2AqzAhIAAQQAgAzYCoMCEgAAMBQtBACgCnMCEgAAiCUUNASAJaEECdEHIwoSAAGooAgAiBygCBEF4cSADayEEIAchBQJAA0ACQCAFKAIQIgANACAFKAIUIgBFDQILIAAoAgRBeHEgA2siBSAEIAUgBEkiBRshBCAAIAcgBRshByAAIQUMAAsLIAdBACgCqMCEgAAiCkkNAiAHKAIYIQsCQAJAIAcoAgwiACAHRg0AIAcoAggiBSAKSQ0EIAUoAgwgB0cNBCAAKAIIIAdHDQQgBSAANgIMIAAgBTYCCAwBCwJAAkACQCAHKAIUIgVFDQAgB0EUaiEIDAELIAcoAhAiBUUNASAHQRBqIQgLA0AgCCEMIAUiAEEUaiEIIAAoAhQiBQ0AIABBEGohCCAAKAIQIgUNAAsgDCAKSQ0EIAxBADYCAAwBC0EAIQALAkAgC0UNAAJAAkAgByAHKAIcIghBAnRByMKEgABqIgUoAgBHDQAgBSAANgIAIAANAUEAIAlBfiAId3E2ApzAhIAADAILIAsgCkkNBAJAAkAgCygCECAHRw0AIAsgADYCEAwBCyALIAA2AhQLIABFDQELIAAgCkkNAyAAIAs2AhgCQCAHKAIQIgVFDQAgBSAKSQ0EIAAgBTYCECAFIAA2AhgLIAcoAhQiBUUNACAFIApJDQMgACAFNgIUIAUgADYCGAsCQAJAIARBD0sNACAHIAQgA2oiAEEDcjYCBCAHIABqIgAgACgCBEEBcjYCBAwBCyAHIANBA3I2AgQgByADaiIDIARBAXI2AgQgAyAEaiAENgIAAkAgBkUNACAGQXhxQcDAhIAAaiEFQQAoAqzAhIAAIQACQAJAQQEgBkEDdnQiCCACcQ0AQQAgCCACcjYCmMCEgAAgBSEIDAELIAUoAggiCCAKSQ0FCyAFIAA2AgggCCAANgIMIAAgBTYCDCAAIAg2AggLQQAgAzYCrMCEgABBACAENgKgwISAAAsgB0EIaiEADAQLQX8hAyAAQb9/Sw0AIABBC2oiBEF4cSEDQQAoApzAhIAAIgtFDQBBHyEGAkAgAEH0//8HSw0AIANBJiAEQQh2ZyIAa3ZBAXEgAEEBdGtBPmohBgtBACADayEEAkACQAJAAkAgBkECdEHIwoSAAGooAgAiBQ0AQQAhAEEAIQgMAQtBACEAIANBAEEZIAZBAXZrIAZBH0YbdCEHQQAhCANAAkAgBSgCBEF4cSADayICIARPDQAgAiEEIAUhCCACDQBBACEEIAUhCCAFIQAMAwsgACAFKAIUIgIgAiAFIAdBHXZBBHFqKAIQIgxGGyAAIAIbIQAgB0EBdCEHIAwhBSAMDQALCwJAIAAgCHINAEEAIQhBAiAGdCIAQQAgAGtyIAtxIgBFDQMgAGhBAnRByMKEgABqKAIAIQALIABFDQELA0AgACgCBEF4cSADayICIARJIQcCQCAAKAIQIgUNACAAKAIUIQULIAIgBCAHGyEEIAAgCCAHGyEIIAUhACAFDQALCyAIRQ0AIARBACgCoMCEgAAgA2tPDQAgCEEAKAKowISAACIMSQ0BIAgoAhghBgJAAkAgCCgCDCIAIAhGDQAgCCgCCCIFIAxJDQMgBSgCDCAIRw0DIAAoAgggCEcNAyAFIAA2AgwgACAFNgIIDAELAkACQAJAIAgoAhQiBUUNACAIQRRqIQcMAQsgCCgCECIFRQ0BIAhBEGohBwsDQCAHIQIgBSIAQRRqIQcgACgCFCIFDQAgAEEQaiEHIAAoAhAiBQ0ACyACIAxJDQMgAkEANgIADAELQQAhAAsCQCAGRQ0AAkACQCAIIAgoAhwiB0ECdEHIwoSAAGoiBSgCAEcNACAFIAA2AgAgAA0BQQAgC0F+IAd3cSILNgKcwISAAAwCCyAGIAxJDQMCQAJAIAYoAhAgCEcNACAGIAA2AhAMAQsgBiAANgIUCyAARQ0BCyAAIAxJDQIgACAGNgIYAkAgCCgCECIFRQ0AIAUgDEkNAyAAIAU2AhAgBSAANgIYCyAIKAIUIgVFDQAgBSAMSQ0CIAAgBTYCFCAFIAA2AhgLAkACQCAEQQ9LDQAgCCAEIANqIgBBA3I2AgQgCCAAaiIAIAAoAgRBAXI2AgQMAQsgCCADQQNyNgIEIAggA2oiByAEQQFyNgIEIAcgBGogBDYCAAJAIARB/wFLDQAgBEF4cUHAwISAAGohAAJAAkBBACgCmMCEgAAiA0EBIARBA3Z0IgRxDQBBACADIARyNgKYwISAACAAIQQMAQsgACgCCCIEIAxJDQQLIAAgBzYCCCAEIAc2AgwgByAANgIMIAcgBDYCCAwBC0EfIQACQCAEQf///wdLDQAgBEEmIARBCHZnIgBrdkEBcSAAQQF0a0E+aiEACyAHIAA2AhwgB0IANwIQIABBAnRByMKEgABqIQMCQAJAAkAgC0EBIAB0IgVxDQBBACALIAVyNgKcwISAACADIAc2AgAgByADNgIYDAELIARBAEEZIABBAXZrIABBH0YbdCEAIAMoAgAhBQNAIAUiAygCBEF4cSAERg0CIABBHXYhBSAAQQF0IQAgAyAFQQRxaiICKAIQIgUNAAsgAkEQaiIAIAxJDQQgACAHNgIAIAcgAzYCGAsgByAHNgIMIAcgBzYCCAwBCyADIAxJDQIgAygCCCIAIAxJDQIgACAHNgIMIAMgBzYCCCAHQQA2AhggByADNgIMIAcgADYCCAsgCEEIaiEADAMLAkBBACgCoMCEgAAiACADSQ0AQQAoAqzAhIAAIQQCQAJAIAAgA2siBUEQSQ0AIAQgA2oiByAFQQFyNgIEIAQgAGogBTYCACAEIANBA3I2AgQMAQsgBCAAQQNyNgIEIAQgAGoiACAAKAIEQQFyNgIEQQAhB0EAIQULQQAgBTYCoMCEgABBACAHNgKswISAACAEQQhqIQAMAwsCQEEAKAKkwISAACIHIANNDQBBACAHIANrIgQ2AqTAhIAAQQBBACgCsMCEgAAiACADaiIFNgKwwISAACAFIARBAXI2AgQgACADQQNyNgIEIABBCGohAAwDCwJAAkBBACgC8MOEgABFDQBBACgC+MOEgAAhBAwBC0EAQn83AvzDhIAAQQBCgKCAgICABDcC9MOEgABBACABQQxqQXBxQdiq1aoFczYC8MOEgABBAEEANgKExISAAEEAQQA2AtTDhIAAQYAgIQQLQQAhACAEIANBL2oiBmoiAkEAIARrIgxxIgggA00NAkEAIQACQEEAKALQw4SAACIERQ0AQQAoAsjDhIAAIgUgCGoiCyAFTQ0DIAsgBEsNAwsCQAJAAkBBAC0A1MOEgABBBHENAAJAAkACQAJAAkBBACgCsMCEgAAiBEUNAEHYw4SAACEAA0ACQCAEIAAoAgAiBUkNACAEIAUgACgCBGpJDQMLIAAoAggiAA0ACwtBABDRgYCAACIHQX9GDQMgCCECAkBBACgC9MOEgAAiAEF/aiIEIAdxRQ0AIAggB2sgBCAHakEAIABrcWohAgsgAiADTQ0DAkBBACgC0MOEgAAiAEUNAEEAKALIw4SAACIEIAJqIgUgBE0NBCAFIABLDQQLIAIQ0YGAgAAiACAHRw0BDAULIAIgB2sgDHEiAhDRgYCAACIHIAAoAgAgACgCBGpGDQEgByEACyAAQX9GDQECQCACIANBMGpJDQAgACEHDAQLIAYgAmtBACgC+MOEgAAiBGpBACAEa3EiBBDRgYCAAEF/Rg0BIAQgAmohAiAAIQcMAwsgB0F/Rw0CC0EAQQAoAtTDhIAAQQRyNgLUw4SAAAsgCBDRgYCAACEHQQAQ0YGAgAAhACAHQX9GDQEgAEF/Rg0BIAcgAE8NASAAIAdrIgIgA0Eoak0NAQtBAEEAKALIw4SAACACaiIANgLIw4SAAAJAIABBACgCzMOEgABNDQBBACAANgLMw4SAAAsCQAJAAkACQEEAKAKwwISAACIERQ0AQdjDhIAAIQADQCAHIAAoAgAiBSAAKAIEIghqRg0CIAAoAggiAA0ADAMLCwJAAkBBACgCqMCEgAAiAEUNACAHIABPDQELQQAgBzYCqMCEgAALQQAhAEEAIAI2AtzDhIAAQQAgBzYC2MOEgABBAEF/NgK4wISAAEEAQQAoAvDDhIAANgK8wISAAEEAQQA2AuTDhIAAA0AgAEEDdCIEQcjAhIAAaiAEQcDAhIAAaiIFNgIAIARBzMCEgABqIAU2AgAgAEEBaiIAQSBHDQALQQAgAkFYaiIAQXggB2tBB3EiBGsiBTYCpMCEgABBACAHIARqIgQ2ArDAhIAAIAQgBUEBcjYCBCAHIABqQSg2AgRBAEEAKAKAxISAADYCtMCEgAAMAgsgBCAHTw0AIAQgBUkNACAAKAIMQQhxDQAgACAIIAJqNgIEQQAgBEF4IARrQQdxIgBqIgU2ArDAhIAAQQBBACgCpMCEgAAgAmoiByAAayIANgKkwISAACAFIABBAXI2AgQgBCAHakEoNgIEQQBBACgCgMSEgAA2ArTAhIAADAELAkAgB0EAKAKowISAAE8NAEEAIAc2AqjAhIAACyAHIAJqIQVB2MOEgAAhAAJAAkADQCAAKAIAIgggBUYNASAAKAIIIgANAAwCCwsgAC0ADEEIcUUNBAtB2MOEgAAhAAJAA0ACQCAEIAAoAgAiBUkNACAEIAUgACgCBGoiBUkNAgsgACgCCCEADAALC0EAIAJBWGoiAEF4IAdrQQdxIghrIgw2AqTAhIAAQQAgByAIaiIINgKwwISAACAIIAxBAXI2AgQgByAAakEoNgIEQQBBACgCgMSEgAA2ArTAhIAAIAQgBUEnIAVrQQdxakFRaiIAIAAgBEEQakkbIghBGzYCBCAIQRBqQQApAuDDhIAANwIAIAhBACkC2MOEgAA3AghBACAIQQhqNgLgw4SAAEEAIAI2AtzDhIAAQQAgBzYC2MOEgABBAEEANgLkw4SAACAIQRhqIQADQCAAQQc2AgQgAEEIaiEHIABBBGohACAHIAVJDQALIAggBEYNACAIIAgoAgRBfnE2AgQgBCAIIARrIgdBAXI2AgQgCCAHNgIAAkACQCAHQf8BSw0AIAdBeHFBwMCEgABqIQACQAJAQQAoApjAhIAAIgVBASAHQQN2dCIHcQ0AQQAgBSAHcjYCmMCEgAAgACEFDAELIAAoAggiBUEAKAKowISAAEkNBQsgACAENgIIIAUgBDYCDEEMIQdBCCEIDAELQR8hAAJAIAdB////B0sNACAHQSYgB0EIdmciAGt2QQFxIABBAXRrQT5qIQALIAQgADYCHCAEQgA3AhAgAEECdEHIwoSAAGohBQJAAkACQEEAKAKcwISAACIIQQEgAHQiAnENAEEAIAggAnI2ApzAhIAAIAUgBDYCACAEIAU2AhgMAQsgB0EAQRkgAEEBdmsgAEEfRht0IQAgBSgCACEIA0AgCCIFKAIEQXhxIAdGDQIgAEEddiEIIABBAXQhACAFIAhBBHFqIgIoAhAiCA0ACyACQRBqIgBBACgCqMCEgABJDQUgACAENgIAIAQgBTYCGAtBCCEHQQwhCCAEIQUgBCEADAELIAVBACgCqMCEgAAiB0kNAyAFKAIIIgAgB0kNAyAAIAQ2AgwgBSAENgIIIAQgADYCCEEAIQBBGCEHQQwhCAsgBCAIaiAFNgIAIAQgB2ogADYCAAtBACgCpMCEgAAiACADTQ0AQQAgACADayIENgKkwISAAEEAQQAoArDAhIAAIgAgA2oiBTYCsMCEgAAgBSAEQQFyNgIEIAAgA0EDcjYCBCAAQQhqIQAMAwsQ74CAgABBMDYCAEEAIQAMAgsQyYGAgAAACyAAIAc2AgAgACAAKAIEIAJqNgIEIAcgCCADEMuBgIAAIQALIAFBEGokgICAgAAgAAuGCgEHfyAAQXggAGtBB3FqIgMgAkEDcjYCBCABQXggAWtBB3FqIgQgAyACaiIFayEAAkACQAJAIARBACgCsMCEgABHDQBBACAFNgKwwISAAEEAQQAoAqTAhIAAIABqIgI2AqTAhIAAIAUgAkEBcjYCBAwBCwJAIARBACgCrMCEgABHDQBBACAFNgKswISAAEEAQQAoAqDAhIAAIABqIgI2AqDAhIAAIAUgAkEBcjYCBCAFIAJqIAI2AgAMAQsCQCAEKAIEIgZBA3FBAUcNACAEKAIMIQICQAJAIAZB/wFLDQACQCAEKAIIIgEgBkEDdiIHQQN0QcDAhIAAaiIIRg0AIAFBACgCqMCEgABJDQUgASgCDCAERw0FCwJAIAIgAUcNAEEAQQAoApjAhIAAQX4gB3dxNgKYwISAAAwCCwJAIAIgCEYNACACQQAoAqjAhIAASQ0FIAIoAgggBEcNBQsgASACNgIMIAIgATYCCAwBCyAEKAIYIQkCQAJAIAIgBEYNACAEKAIIIgFBACgCqMCEgABJDQUgASgCDCAERw0FIAIoAgggBEcNBSABIAI2AgwgAiABNgIIDAELAkACQAJAIAQoAhQiAUUNACAEQRRqIQgMAQsgBCgCECIBRQ0BIARBEGohCAsDQCAIIQcgASICQRRqIQggAigCFCIBDQAgAkEQaiEIIAIoAhAiAQ0ACyAHQQAoAqjAhIAASQ0FIAdBADYCAAwBC0EAIQILIAlFDQACQAJAIAQgBCgCHCIIQQJ0QcjChIAAaiIBKAIARw0AIAEgAjYCACACDQFBAEEAKAKcwISAAEF+IAh3cTYCnMCEgAAMAgsgCUEAKAKowISAAEkNBAJAAkAgCSgCECAERw0AIAkgAjYCEAwBCyAJIAI2AhQLIAJFDQELIAJBACgCqMCEgAAiCEkNAyACIAk2AhgCQCAEKAIQIgFFDQAgASAISQ0EIAIgATYCECABIAI2AhgLIAQoAhQiAUUNACABIAhJDQMgAiABNgIUIAEgAjYCGAsgBkF4cSICIABqIQAgBCACaiIEKAIEIQYLIAQgBkF+cTYCBCAFIABBAXI2AgQgBSAAaiAANgIAAkAgAEH/AUsNACAAQXhxQcDAhIAAaiECAkACQEEAKAKYwISAACIBQQEgAEEDdnQiAHENAEEAIAEgAHI2ApjAhIAAIAIhAAwBCyACKAIIIgBBACgCqMCEgABJDQMLIAIgBTYCCCAAIAU2AgwgBSACNgIMIAUgADYCCAwBC0EfIQICQCAAQf///wdLDQAgAEEmIABBCHZnIgJrdkEBcSACQQF0a0E+aiECCyAFIAI2AhwgBUIANwIQIAJBAnRByMKEgABqIQECQAJAAkBBACgCnMCEgAAiCEEBIAJ0IgRxDQBBACAIIARyNgKcwISAACABIAU2AgAgBSABNgIYDAELIABBAEEZIAJBAXZrIAJBH0YbdCECIAEoAgAhCANAIAgiASgCBEF4cSAARg0CIAJBHXYhCCACQQF0IQIgASAIQQRxaiIEKAIQIggNAAsgBEEQaiICQQAoAqjAhIAASQ0DIAIgBTYCACAFIAE2AhgLIAUgBTYCDCAFIAU2AggMAQsgAUEAKAKowISAACIASQ0BIAEoAggiAiAASQ0BIAIgBTYCDCABIAU2AgggBUEANgIYIAUgATYCDCAFIAI2AggLIANBCGoPCxDJgYCAAAALvQ8BCn8CQAJAIABFDQAgAEF4aiIBQQAoAqjAhIAAIgJJDQEgAEF8aigCACIDQQNxQQFGDQEgASADQXhxIgBqIQQCQCADQQFxDQAgA0ECcUUNASABIAEoAgAiBWsiASACSQ0CIAUgAGohAAJAIAFBACgCrMCEgABGDQAgASgCDCEDAkAgBUH/AUsNAAJAIAEoAggiBiAFQQN2IgdBA3RBwMCEgABqIgVGDQAgBiACSQ0FIAYoAgwgAUcNBQsCQCADIAZHDQBBAEEAKAKYwISAAEF+IAd3cTYCmMCEgAAMAwsCQCADIAVGDQAgAyACSQ0FIAMoAgggAUcNBQsgBiADNgIMIAMgBjYCCAwCCyABKAIYIQgCQAJAIAMgAUYNACABKAIIIgUgAkkNBSAFKAIMIAFHDQUgAygCCCABRw0FIAUgAzYCDCADIAU2AggMAQsCQAJAAkAgASgCFCIFRQ0AIAFBFGohBgwBCyABKAIQIgVFDQEgAUEQaiEGCwNAIAYhByAFIgNBFGohBiADKAIUIgUNACADQRBqIQYgAygCECIFDQALIAcgAkkNBSAHQQA2AgAMAQtBACEDCyAIRQ0BAkACQCABIAEoAhwiBkECdEHIwoSAAGoiBSgCAEcNACAFIAM2AgAgAw0BQQBBACgCnMCEgABBfiAGd3E2ApzAhIAADAMLIAggAkkNBAJAAkAgCCgCECABRw0AIAggAzYCEAwBCyAIIAM2AhQLIANFDQILIAMgAkkNAyADIAg2AhgCQCABKAIQIgVFDQAgBSACSQ0EIAMgBTYCECAFIAM2AhgLIAEoAhQiBUUNASAFIAJJDQMgAyAFNgIUIAUgAzYCGAwBCyAEKAIEIgNBA3FBA0cNAEEAIAA2AqDAhIAAIAQgA0F+cTYCBCABIABBAXI2AgQgBCAANgIADwsgASAETw0BIAQoAgQiB0EBcUUNAQJAAkAgB0ECcQ0AAkAgBEEAKAKwwISAAEcNAEEAIAE2ArDAhIAAQQBBACgCpMCEgAAgAGoiADYCpMCEgAAgASAAQQFyNgIEIAFBACgCrMCEgABHDQNBAEEANgKgwISAAEEAQQA2AqzAhIAADwsCQCAEQQAoAqzAhIAAIglHDQBBACABNgKswISAAEEAQQAoAqDAhIAAIABqIgA2AqDAhIAAIAEgAEEBcjYCBCABIABqIAA2AgAPCyAEKAIMIQMCQAJAIAdB/wFLDQACQCAEKAIIIgUgB0EDdiIIQQN0QcDAhIAAaiIGRg0AIAUgAkkNBiAFKAIMIARHDQYLAkAgAyAFRw0AQQBBACgCmMCEgABBfiAId3E2ApjAhIAADAILAkAgAyAGRg0AIAMgAkkNBiADKAIIIARHDQYLIAUgAzYCDCADIAU2AggMAQsgBCgCGCEKAkACQCADIARGDQAgBCgCCCIFIAJJDQYgBSgCDCAERw0GIAMoAgggBEcNBiAFIAM2AgwgAyAFNgIIDAELAkACQAJAIAQoAhQiBUUNACAEQRRqIQYMAQsgBCgCECIFRQ0BIARBEGohBgsDQCAGIQggBSIDQRRqIQYgAygCFCIFDQAgA0EQaiEGIAMoAhAiBQ0ACyAIIAJJDQYgCEEANgIADAELQQAhAwsgCkUNAAJAAkAgBCAEKAIcIgZBAnRByMKEgABqIgUoAgBHDQAgBSADNgIAIAMNAUEAQQAoApzAhIAAQX4gBndxNgKcwISAAAwCCyAKIAJJDQUCQAJAIAooAhAgBEcNACAKIAM2AhAMAQsgCiADNgIUCyADRQ0BCyADIAJJDQQgAyAKNgIYAkAgBCgCECIFRQ0AIAUgAkkNBSADIAU2AhAgBSADNgIYCyAEKAIUIgVFDQAgBSACSQ0EIAMgBTYCFCAFIAM2AhgLIAEgB0F4cSAAaiIAQQFyNgIEIAEgAGogADYCACABIAlHDQFBACAANgKgwISAAA8LIAQgB0F+cTYCBCABIABBAXI2AgQgASAAaiAANgIACwJAIABB/wFLDQAgAEF4cUHAwISAAGohAwJAAkBBACgCmMCEgAAiBUEBIABBA3Z0IgBxDQBBACAFIAByNgKYwISAACADIQAMAQsgAygCCCIAIAJJDQMLIAMgATYCCCAAIAE2AgwgASADNgIMIAEgADYCCA8LQR8hAwJAIABB////B0sNACAAQSYgAEEIdmciA2t2QQFxIANBAXRrQT5qIQMLIAEgAzYCHCABQgA3AhAgA0ECdEHIwoSAAGohBgJAAkACQAJAQQAoApzAhIAAIgVBASADdCIEcQ0AQQAgBSAEcjYCnMCEgAAgBiABNgIAQQghAEEYIQMMAQsgAEEAQRkgA0EBdmsgA0EfRht0IQMgBigCACEGA0AgBiIFKAIEQXhxIABGDQIgA0EddiEGIANBAXQhAyAFIAZBBHFqIgQoAhAiBg0ACyAEQRBqIgAgAkkNBCAAIAE2AgBBCCEAQRghAyAFIQYLIAEhBSABIQQMAQsgBSACSQ0CIAUoAggiBiACSQ0CIAYgATYCDCAFIAE2AghBACEEQRghAEEIIQMLIAEgA2ogBjYCACABIAU2AgwgASAAaiAENgIAQQBBACgCuMCEgABBf2oiAUF/IAEbNgK4wISAAAsPCxDJgYCAAAALngEBAn8CQCAADQAgARDKgYCAAA8LAkAgAUFASQ0AEO+AgIAAQTA2AgBBAA8LAkAgAEF4akEQIAFBC2pBeHEgAUELSRsQzoGAgAAiAkUNACACQQhqDwsCQCABEMqBgIAAIgINAEEADwsgAiAAQXxBeCAAQXxqKAIAIgNBA3EbIANBeHFqIgMgASADIAFJGxCIgYCAABogABDMgYCAACACC5EJAQl/AkACQCAAQQAoAqjAhIAAIgJJDQAgACgCBCIDQQNxIgRBAUYNACADQXhxIgVFDQAgACAFaiIGKAIEIgdBAXFFDQACQCAEDQBBACEEIAFBgAJJDQICQCAFIAFBBGpJDQAgACEEIAUgAWtBACgC+MOEgABBAXRNDQMLQQAhBAwCCwJAIAUgAUkNAAJAIAUgAWsiBUEQSQ0AIAAgASADQQFxckECcjYCBCAAIAFqIgEgBUEDcjYCBCAGIAYoAgRBAXI2AgQgASAFEM+BgIAACyAADwtBACEEAkAgBkEAKAKwwISAAEcNAEEAKAKkwISAACAFaiIFIAFNDQIgACABIANBAXFyQQJyNgIEIAAgAWoiAyAFIAFrIgVBAXI2AgRBACAFNgKkwISAAEEAIAM2ArDAhIAAIAAPCwJAIAZBACgCrMCEgABHDQBBACEEQQAoAqDAhIAAIAVqIgUgAUkNAgJAAkAgBSABayIEQRBJDQAgACABIANBAXFyQQJyNgIEIAAgAWoiASAEQQFyNgIEIAAgBWoiBSAENgIAIAUgBSgCBEF+cTYCBAwBCyAAIANBAXEgBXJBAnI2AgQgACAFaiIFIAUoAgRBAXI2AgRBACEEQQAhAQtBACABNgKswISAAEEAIAQ2AqDAhIAAIAAPC0EAIQQgB0ECcQ0BIAdBeHEgBWoiCCABSQ0BIAYoAgwhBQJAAkAgB0H/AUsNAAJAIAYoAggiBCAHQQN2IglBA3RBwMCEgABqIgdGDQAgBCACSQ0DIAQoAgwgBkcNAwsCQCAFIARHDQBBAEEAKAKYwISAAEF+IAl3cTYCmMCEgAAMAgsCQCAFIAdGDQAgBSACSQ0DIAUoAgggBkcNAwsgBCAFNgIMIAUgBDYCCAwBCyAGKAIYIQoCQAJAIAUgBkYNACAGKAIIIgQgAkkNAyAEKAIMIAZHDQMgBSgCCCAGRw0DIAQgBTYCDCAFIAQ2AggMAQsCQAJAAkAgBigCFCIERQ0AIAZBFGohBwwBCyAGKAIQIgRFDQEgBkEQaiEHCwNAIAchCSAEIgVBFGohByAFKAIUIgQNACAFQRBqIQcgBSgCECIEDQALIAkgAkkNAyAJQQA2AgAMAQtBACEFCyAKRQ0AAkACQCAGIAYoAhwiB0ECdEHIwoSAAGoiBCgCAEcNACAEIAU2AgAgBQ0BQQBBACgCnMCEgABBfiAHd3E2ApzAhIAADAILIAogAkkNAgJAAkAgCigCECAGRw0AIAogBTYCEAwBCyAKIAU2AhQLIAVFDQELIAUgAkkNASAFIAo2AhgCQCAGKAIQIgRFDQAgBCACSQ0CIAUgBDYCECAEIAU2AhgLIAYoAhQiBEUNACAEIAJJDQEgBSAENgIUIAQgBTYCGAsCQCAIIAFrIgVBD0sNACAAIANBAXEgCHJBAnI2AgQgACAIaiIFIAUoAgRBAXI2AgQgAA8LIAAgASADQQFxckECcjYCBCAAIAFqIgEgBUEDcjYCBCAAIAhqIgMgAygCBEEBcjYCBCABIAUQz4GAgAAgAA8LEMmBgIAAAAsgBAvxDgEJfyAAIAFqIQICQAJAAkACQCAAKAIEIgNBAXFFDQBBACgCqMCEgAAhBAwBCyADQQJxRQ0BIAAgACgCACIFayIAQQAoAqjAhIAAIgRJDQIgBSABaiEBAkAgAEEAKAKswISAAEYNACAAKAIMIQMCQCAFQf8BSw0AAkAgACgCCCIGIAVBA3YiB0EDdEHAwISAAGoiBUYNACAGIARJDQUgBigCDCAARw0FCwJAIAMgBkcNAEEAQQAoApjAhIAAQX4gB3dxNgKYwISAAAwDCwJAIAMgBUYNACADIARJDQUgAygCCCAARw0FCyAGIAM2AgwgAyAGNgIIDAILIAAoAhghCAJAAkAgAyAARg0AIAAoAggiBSAESQ0FIAUoAgwgAEcNBSADKAIIIABHDQUgBSADNgIMIAMgBTYCCAwBCwJAAkACQCAAKAIUIgVFDQAgAEEUaiEGDAELIAAoAhAiBUUNASAAQRBqIQYLA0AgBiEHIAUiA0EUaiEGIAMoAhQiBQ0AIANBEGohBiADKAIQIgUNAAsgByAESQ0FIAdBADYCAAwBC0EAIQMLIAhFDQECQAJAIAAgACgCHCIGQQJ0QcjChIAAaiIFKAIARw0AIAUgAzYCACADDQFBAEEAKAKcwISAAEF+IAZ3cTYCnMCEgAAMAwsgCCAESQ0EAkACQCAIKAIQIABHDQAgCCADNgIQDAELIAggAzYCFAsgA0UNAgsgAyAESQ0DIAMgCDYCGAJAIAAoAhAiBUUNACAFIARJDQQgAyAFNgIQIAUgAzYCGAsgACgCFCIFRQ0BIAUgBEkNAyADIAU2AhQgBSADNgIYDAELIAIoAgQiA0EDcUEDRw0AQQAgATYCoMCEgAAgAiADQX5xNgIEIAAgAUEBcjYCBCACIAE2AgAPCyACIARJDQECQAJAIAIoAgQiCEECcQ0AAkAgAkEAKAKwwISAAEcNAEEAIAA2ArDAhIAAQQBBACgCpMCEgAAgAWoiATYCpMCEgAAgACABQQFyNgIEIABBACgCrMCEgABHDQNBAEEANgKgwISAAEEAQQA2AqzAhIAADwsCQCACQQAoAqzAhIAAIglHDQBBACAANgKswISAAEEAQQAoAqDAhIAAIAFqIgE2AqDAhIAAIAAgAUEBcjYCBCAAIAFqIAE2AgAPCyACKAIMIQMCQAJAIAhB/wFLDQACQCACKAIIIgUgCEEDdiIHQQN0QcDAhIAAaiIGRg0AIAUgBEkNBiAFKAIMIAJHDQYLAkAgAyAFRw0AQQBBACgCmMCEgABBfiAHd3E2ApjAhIAADAILAkAgAyAGRg0AIAMgBEkNBiADKAIIIAJHDQYLIAUgAzYCDCADIAU2AggMAQsgAigCGCEKAkACQCADIAJGDQAgAigCCCIFIARJDQYgBSgCDCACRw0GIAMoAgggAkcNBiAFIAM2AgwgAyAFNgIIDAELAkACQAJAIAIoAhQiBUUNACACQRRqIQYMAQsgAigCECIFRQ0BIAJBEGohBgsDQCAGIQcgBSIDQRRqIQYgAygCFCIFDQAgA0EQaiEGIAMoAhAiBQ0ACyAHIARJDQYgB0EANgIADAELQQAhAwsgCkUNAAJAAkAgAiACKAIcIgZBAnRByMKEgABqIgUoAgBHDQAgBSADNgIAIAMNAUEAQQAoApzAhIAAQX4gBndxNgKcwISAAAwCCyAKIARJDQUCQAJAIAooAhAgAkcNACAKIAM2AhAMAQsgCiADNgIUCyADRQ0BCyADIARJDQQgAyAKNgIYAkAgAigCECIFRQ0AIAUgBEkNBSADIAU2AhAgBSADNgIYCyACKAIUIgVFDQAgBSAESQ0EIAMgBTYCFCAFIAM2AhgLIAAgCEF4cSABaiIBQQFyNgIEIAAgAWogATYCACAAIAlHDQFBACABNgKgwISAAA8LIAIgCEF+cTYCBCAAIAFBAXI2AgQgACABaiABNgIACwJAIAFB/wFLDQAgAUF4cUHAwISAAGohAwJAAkBBACgCmMCEgAAiBUEBIAFBA3Z0IgFxDQBBACAFIAFyNgKYwISAACADIQEMAQsgAygCCCIBIARJDQMLIAMgADYCCCABIAA2AgwgACADNgIMIAAgATYCCA8LQR8hAwJAIAFB////B0sNACABQSYgAUEIdmciA2t2QQFxIANBAXRrQT5qIQMLIAAgAzYCHCAAQgA3AhAgA0ECdEHIwoSAAGohBQJAAkACQEEAKAKcwISAACIGQQEgA3QiAnENAEEAIAYgAnI2ApzAhIAAIAUgADYCACAAIAU2AhgMAQsgAUEAQRkgA0EBdmsgA0EfRht0IQMgBSgCACEGA0AgBiIFKAIEQXhxIAFGDQIgA0EddiEGIANBAXQhAyAFIAZBBHFqIgIoAhAiBg0ACyACQRBqIgEgBEkNAyABIAA2AgAgACAFNgIYCyAAIAA2AgwgACAANgIIDwsgBSAESQ0BIAUoAggiASAESQ0BIAEgADYCDCAFIAA2AgggAEEANgIYIAAgBTYCDCAAIAE2AggLDwsQyYGAgAAACwcAPwBBEHQLYQECf0EAKALMsoSAACIBIABBB2pBeHEiAmohAAJAAkACQCACRQ0AIAAgAU0NAQsgABDQgYCAAE0NASAAELWAgIAADQELEO+AgIAAQTA2AgBBfw8LQQAgADYCzLKEgAAgAQtTAQF+AkACQCADQcAAcUUNACABIANBQGqthiECQgAhAQwBCyADRQ0AIAFBwAAgA2utiCACIAOtIgSGhCECIAEgBIYhAQsgACABNwMAIAAgAjcDCAtTAQF+AkACQCADQcAAcUUNACACIANBQGqtiCEBQgAhAgwBCyADRQ0AIAJBwAAgA2uthiABIAOtIgSIhCEBIAIgBIghAgsgACABNwMAIAAgAjcDCAubBAMBfwJ+BH8jgICAgABBIGsiAiSAgICAACABQv///////z+DIQMCQAJAIAFCMIhC//8BgyIEpyIFQf+Hf2pB/Q9LDQAgAEI8iCADQgSGhCEDIAVBgIh/aq0hBAJAAkAgAEL//////////w+DIgBCgYCAgICAgIAIVA0AIANCAXwhAwwBCyAAQoCAgICAgICACFINACADQgGDIAN8IQMLQgAgAyADQv////////8HViIFGyEAIAWtIAR8IQMMAQsCQCAAIAOEUA0AIARC//8BUg0AIABCPIggA0IEhoRCgICAgICAgASEIQBC/w8hAwwBCwJAIAVB/ocBTQ0AQv8PIQNCACEADAELAkBBgPgAQYH4ACAEUCIGGyIHIAVrIghB8ABMDQBCACEAQgAhAwwBCyACQRBqIAAgAyADQoCAgICAgMAAhCAGGyIDQYABIAhrENKBgIAAIAIgACADIAgQ04GAgAAgAikDACIDQjyIIAIpAwhCBIaEIQACQAJAIANC//////////8PgyAHIAVHIAIpAxAgAikDGIRCAFJxrYQiA0KBgICAgICAgAhUDQAgAEIBfCEADAELIANCgICAgICAgIAIUg0AIABCAYMgAHwhAAsgAEKAgICAgICACIUgACAAQv////////8HViIFGyEAIAWtIQMLIAJBIGokgICAgAAgA0I0hiABQoCAgICAgICAgH+DhCAAhL8LJwACQCAARQ0AQYqChIAAQaeChIAAQRhBu4OEgAAQtoCAgAAAC0EBCwIACwoAIAAkgICAgAALGgECfyOAgICAACAAa0FwcSIBJICAgIAAIAELCAAjgICAgAALIABBgICEgAAk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IHR0eQBQZXJtaXNzaW9uIGRlbmllZABPcGVyYXRpb24gbm90IHBlcm1pdHRlZABObyBzdWNoIGZpbGUgb3IgZGlyZWN0b3J5AE5vIHN1Y2ggcHJvY2VzcwBGaWxlIGV4aXN0cwBWYWx1ZSB0b28gbGFyZ2UgZm9yIGRhdGEgdHlwZQBObyBzcGFjZSBsZWZ0IG9uIGRldmljZQBPdXQgb2YgbWVtb3J5AFJlc291cmNlIGJ1c3kASW50ZXJydXB0ZWQgc3lzdGVtIGNhbGwAUmVzb3VyY2UgdGVtcG9yYXJpbHkgdW5hdmFpbGFibGUASW52YWxpZCBzZWVrAENyb3NzLWRldmljZSBsaW5rAFJlYWQtb25seSBmaWxlIHN5c3RlbQBEaXJlY3Rvcnkgbm90IGVtcHR5AENvbm5lY3Rpb24gcmVzZXQgYnkgcGVlcgBPcGVyYXRpb24gdGltZWQgb3V0AENvbm5lY3Rpb24gcmVmdXNlZABIb3N0IGlzIGRvd24ASG9zdCBpcyB1bnJlYWNoYWJsZQBBZGRyZXNzIGluIHVzZQBCcm9rZW4gcGlwZQBJL08gZXJyb3IATm8gc3VjaCBkZXZpY2Ugb3IgYWRkcmVzcwBCbG9jayBkZXZpY2UgcmVxdWlyZWQATm8gc3VjaCBkZXZpY2UATm90IGEgZGlyZWN0b3J5AElzIGEgZGlyZWN0b3J5AFRleHQgZmlsZSBidXN5AEV4ZWMgZm9ybWF0IGVycm9yAEludmFsaWQgYXJndW1lbnQAQXJndW1lbnQgbGlzdCB0b28gbG9uZwBTeW1ib2xpYyBsaW5rIGxvb3AARmlsZW5hbWUgdG9vIGxvbmcAVG9vIG1hbnkgb3BlbiBmaWxlcyBpbiBzeXN0ZW0ATm8gZmlsZSBkZXNjcmlwdG9ycyBhdmFpbGFibGUAQmFkIGZpbGUgZGVzY3JpcHRvcgBObyBjaGlsZCBwcm9jZXNzAEJhZCBhZGRyZXNzAEZpbGUgdG9vIGxhcmdlAFRvbyBtYW55IGxpbmtzAE5vIGxvY2tzIGF2YWlsYWJsZQBSZXNvdXJjZSBkZWFkbG9jayB3b3VsZCBvY2N1cgBTdGF0ZSBub3QgcmVjb3ZlcmFibGUAUHJldmlvdXMgb3duZXIgZGllZABPcGVyYXRpb24gY2FuY2VsZWQARnVuY3Rpb24gbm90IGltcGxlbWVudGVkAE5vIG1lc3NhZ2Ugb2YgZGVzaXJlZCB0eXBlAElkZW50aWZpZXIgcmVtb3ZlZABEZXZpY2Ugbm90IGEgc3RyZWFtAE5vIGRhdGEgYXZhaWxhYmxlAERldmljZSB0aW1lb3V0AE91dCBvZiBzdHJlYW1zIHJlc291cmNlcwBMaW5rIGhhcyBiZWVuIHNldmVyZWQAUHJvdG9jb2wgZXJyb3IAQmFkIG1lc3NhZ2UARmlsZSBkZXNjcmlwdG9yIGluIGJhZCBzdGF0ZQBOb3QgYSBzb2NrZXQARGVzdGluYXRpb24gYWRkcmVzcyByZXF1aXJlZABNZXNzYWdlIHRvbyBsYXJnZQBQcm90b2NvbCB3cm9uZyB0eXBlIGZvciBzb2NrZXQAUHJvdG9jb2wgbm90IGF2YWlsYWJsZQBQcm90b2NvbCBub3Qgc3VwcG9ydGVkAFNvY2tldCB0eXBlIG5vdCBzdXBwb3J0ZWQATm90IHN1cHBvcnRlZABQcm90b2NvbCBmYW1pbHkgbm90IHN1cHBvcnRlZABBZGRyZXNzIGZhbWlseSBub3Qgc3VwcG9ydGVkIGJ5IHByb3RvY29sAEFkZHJlc3Mgbm90IGF2YWlsYWJsZQBOZXR3b3JrIGlzIGRvd24ATmV0d29yayB1bnJlYWNoYWJsZQBDb25uZWN0aW9uIHJlc2V0IGJ5IG5ldHdvcmsAQ29ubmVjdGlvbiBhYm9ydGVkAE5vIGJ1ZmZlciBzcGFjZSBhdmFpbGFibGUAU29ja2V0IGlzIGNvbm5lY3RlZABTb2NrZXQgbm90IGNvbm5lY3RlZABDYW5ub3Qgc2VuZCBhZnRlciBzb2NrZXQgc2h1dGRvd24AT3BlcmF0aW9uIGFscmVhZHkgaW4gcHJvZ3Jlc3MAT3BlcmF0aW9uIGluIHByb2dyZXNzAFN0YWxlIGZpbGUgaGFuZGxlAFJlbW90ZSBJL08gZXJyb3IAUXVvdGEgZXhjZWVkZWQATm8gbWVkaXVtIGZvdW5kAFdyb25nIG1lZGl1bSB0eXBlAE11bHRpaG9wIGF0dGVtcHRlZABSZXF1aXJlZCBrZXkgbm90IGF2YWlsYWJsZQBLZXkgaGFzIGV4cGlyZWQAS2V5IGhhcyBiZWVuIHJldm9rZWQAS2V5IHdhcyByZWplY3RlZCBieSBzZXJ2aWNlAAAAAAClAlsA8AG1BYwFJQGDBh0DlAT/AMcDMQMLBrwBjwF/A8oEKwDaBq8AQgNOA9wBDgQVAKEGDQGUAgsCOAZkArwC/wJdA+cECwfPAssF7wXbBeECHgZFAoUAggJsA28E8QDzAxgF2QDaA0wGVAJ7AZ0DvQQAAFEAFQK7ALMDbQD/AYUELwX5BDgAZQFGAZ8AtwaoAXMCUwEAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAhBAAAAAAAAAAALwIAAAAAAAAAAAAAAAAAAAAAAAAAADUERwRWBAAAAAAAAAAAAAAAAAAAAACgBAAAAAAAAAAAAAAAAAAAAAAAAEYFYAVuBWEGAADPAQAAAAAAAAAAyQbpBvkGHgc5B0kHXgcAAAAAAAAAAAAAAAAZAAsAGRkZAAAAAAUAAAAAAAAJAAAAAAsAAAAAAAAAABkACgoZGRkDCgcAAQAJCxgAAAkGCwAACwAGGQAAABkZGQAAAAAAAAAAAAAAAAAAAAAOAAAAAAAAAAAZAAsNGRkZAA0AAAIACQ4AAAAJAA4AAA4AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAADAAAAAAAAAAAAAAAEwAAAAATAAAAAAkMAAAAAAAMAAAMAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAABAAAAAAAAAAAAAAAA8AAAAEDwAAAAAJEAAAAAAAEAAAEAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAASAAAAAAAAAAAAAAARAAAAABEAAAAACRIAAAAAABIAABIAABoAAAAaGhoAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAGgAAABoaGgAAAAAAAAkAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAABQAAAAAAAAAAAAAABcAAAAAFwAAAAAJFAAAAAAAFAAAFAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAWAAAAAAAAAAAAAAAVAAAAABUAAAAACRYAAAAAABYAABYAADAxMjM0NTY3ODlBQkNERUYAQZCwBAvAAgAAAQACAAAAAgADAAAAAAAFAAAAAAAAAAAAAAALAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAJAAAACAAAABAcAQAAAAAAAAAAAAAAAAACAAAAAAAAAAAAAAAAAAAA//////////8AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAgGAEAAAAAAAUAAAAAAAAAAAAAAAwAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAkAAAANAAAAGBwBAAAEAAAAAAAAAAAAAAEAAAAAAAAAAAAAAAAAAAD/////CgAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAALgYAQAQIgEAAJQBD3RhcmdldF9mZWF0dXJlcwgrC2J1bGstbWVtb3J5Kw9idWxrLW1lbW9yeS1vcHQrFmNhbGwtaW5kaXJlY3Qtb3ZlcmxvbmcrCm11bHRpdmFsdWUrD211dGFibGUtZ2xvYmFscysTbm9udHJhcHBpbmctZnB0b2ludCsPcmVmZXJlbmNlLXR5cGVzKwhzaWduLWV4dA==';

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

