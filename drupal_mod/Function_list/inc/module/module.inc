<?php 
/*
module_implements
7 module.inc 	module_implements($hook, $sort = FALSE, $reset = FALSE)
4.6 module.inc 	module_implements($hook)
4.7 module.inc 	module_implements($hook, $sort = FALSE)
5 module.inc 	module_implements($hook, $sort = FALSE, $refresh = FALSE)
6 module.inc 	module_implements($hook, $sort = FALSE, $refresh = FALSE)
8 bootstrap.inc 	module_implements($hook)

Determines which modules are implementing a hook.
Parameters

$hook: The name of the hook (e.g. "help" or "menu").

$sort: By default, modules are ordered by weight and filename, settings this option to TRUE, module list will be ordered by module name.

$reset: For internal use only: Whether to force the stored list of hook implementations to be regenerated (such as after enabling a new module, before processing hook_enable).
Return value

An array with the names of the modules which are implementing this hook.
See also

module_implements_write_cache()
Related topics

Hooks
    Allow modules to interact with the Drupal core.

66 calls to module_implements()
4 string references to 'module_implements'
File

includes/module.inc, line 663
    API for loading and interacting with Drupal modules.

Code
*/
function module_implements($hook, $sort = FALSE, $reset = FALSE) {
  // Use the advanced drupal_static() pattern, since this is called very often.
  static $drupal_static_fast;
  if (!isset($drupal_static_fast)) {
    $drupal_static_fast['implementations'] = &drupal_static(__FUNCTION__);
  }
  $implementations = &$drupal_static_fast['implementations'];

  // We maintain a persistent cache of hook implementations in addition to the
  // static cache to avoid looping through every module and every hook on each
  // request. Benchmarks show that the benefit of this caching outweighs the
  // additional database hit even when using the default database caching
  // backend and only a small number of modules are enabled. The cost of the
  // cache_get() is more or less constant and reduced further when non-database
  // caching backends are used, so there will be more significant gains when a
  // large number of modules are installed or hooks invoked, since this can
  // quickly lead to module_hook() being called several thousand times
  // per request.
  if ($reset) {
    $implementations = array();
    cache_set('module_implements', array(), 'cache_bootstrap');
    drupal_static_reset('module_hook_info');
    drupal_static_reset('drupal_alter');
    cache_clear_all('hook_info', 'cache_bootstrap');
    return;
  }

  // Fetch implementations from cache.
  if (empty($implementations)) {
    $implementations = cache_get('module_implements', 'cache_bootstrap');
    if ($implementations === FALSE) {
      $implementations = array();
    }
    else {
      $implementations = $implementations->data;
    }
  }

  if (!isset($implementations[$hook])) {
    // The hook is not cached, so ensure that whether or not it has
    // implementations, that the cache is updated at the end of the request.
    $implementations['#write_cache'] = TRUE;
    $hook_info = module_hook_info();
    $implementations[$hook] = array();
    $list = module_list(FALSE, FALSE, $sort);
    foreach ($list as $module) {
      $include_file = isset($hook_info[$hook]['group']) && module_load_include('inc', $module, $module . '.' . $hook_info[$hook]['group']);
      // Since module_hook() may needlessly try to load the include file again,
      // function_exists() is used directly here.
      if (function_exists($module . '_' . $hook)) {
        $implementations[$hook][$module] = $include_file ? $hook_info[$hook]['group'] : FALSE;
      }
    }
    // Allow modules to change the weight of specific implementations but avoid
    // an infinite loop.
    if ($hook != 'module_implements_alter') {
      drupal_alter('module_implements', $implementations[$hook], $hook);
    }
  }
  else {
    foreach ($implementations[$hook] as $module => $group) {
      // If this hook implementation is stored in a lazy-loaded file, so include
      // that file first.
      if ($group) {
        module_load_include('inc', $module, "$module.$group");
      }
      // It is possible that a module removed a hook implementation without the
      // implementations cache being rebuilt yet, so we check whether the
      // function exists on each request to avoid undefined function errors.
      // Since module_hook() may needlessly try to load the include file again,
      // function_exists() is used directly here.
      if (!function_exists($module . '_' . $hook)) {
        // Clear out the stale implementation from the cache and force a cache
        // refresh to forget about no longer existing hook implementations.
        unset($implementations[$hook][$module]);
        $implementations['#write_cache'] = TRUE;
      }
    }
  }

  return array_keys($implementations[$hook]);
}

function ekmod_module_implements() {
  // Use the advanced drupal_static() pattern, since this is called very often.
  static $drupal_static_fast;
  if (!isset($drupal_static_fast)) {
    $drupal_static_fast['implementations'] = &drupal_static(__FUNCTION__);
  }
  $implementations = &$drupal_static_fast['implementations'];
  
}
?>