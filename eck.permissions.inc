<?php
/**
 * @file
 * Permissions.
 */

/**
 * Permission label.
 *
 * @param string $string
 *   A permission string.
 *
 * @return string
 *   A label created from the permission string.a
 */
function eck_permission_label($string) {

  $replacements = array(
    "eck" => "",
    "*" => "administer",
    "bundle" => "bundles",
    "_" => " ",
  );

  $label = $string;

  foreach ($replacements as $current => $final) {
    $label = str_replace($current, $final, $label);
  }

  if (substr_count($label, "type") > 0) {
    $label = str_replace("entity type", "Entity Types", $label);
  }
  else {
    $label = str_replace("entity", "Entities", $label);
  }

  $label = trim($label);
  $label = ucwords($label);

  return $label;
}

/**
 * Implements hook_permission().
 *
 * A pretty through set of permission
 * you can set permissiona at each level: entity_type, bundle, entity
 * and for each action of the CRUD
 */
function eck_permission() {
  $perms = array();

  $actions = array('*', 'list', 'create', "update", "delete", "view");
  $objects = array("entity_type", "bundle", "entity");

  foreach ($objects as $object) {
    foreach ($actions as $action) {
      $perm = "eck {$action} {$object}";
      $perms[$perm] = array("title" => eck_permission_label($perm));
    }
  }

  // Properties permissions.
  $perm = "eck update property";
  $perms[$perm] = array("title" => "Update Properties");

  return $perms;
}

/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

/**
 * Default access callback for ECK entities.
 *
 * This is used as the default behavior when entity_access() is called to check
 * access on an entity provided by ECK. Note that in addition to calls from
 * within this own module, entity_access() is also called by other modules such
 * as Rules, Entity Reference, etc. (although it is not consistently called
 * throughout Drupal).
 *
 * @param string $op
 *   The operation being performed. One of 'view', 'update', 'create' or
 *   'delete'.
 * @param object $entity
 *   An entity to check access for. If this is NULL, we are checking
 *   access for all entities of the given type. If this is a string
 *   (representing the bundle to check access for; see parallel example in
 *   node_access()) we are checking access for all entities of the given type
 *   and bundle.
 * @param object $account
 *   The user to check access for. If this is NULL, access will be checked for
 *   the current user.
 * @param string $entity_type_name
 *   A string representing the type of entity to check access for.
 *
 * @return bool
 *   TRUE if access is granted, FALSE otherwise.
 *
 * @see entity_access()
 */
function eck_entity_access($op, $entity, $account, $entity_type_name) {
  return eck_access($op, 'entity', $entity, $account);
}

/**
 * Access callback for eck related permissions.
 *
 * @param string $op
 *   Operation (ex. create, list, etc.)
 * @param string $object_type
 *   entity, bundle, entity_type, etc.
 * @param object $object
 *   The object for which the permission is relevant.
 * @param object $account
 *   A user account.
 *
 * @return bool
 *   Is the user allowed or not.
 */
function eck_access($op, $object_type = 'entity', $object = NULL, $account = NULL) {
  $allow = FALSE;

  if (!$account) {
    global $user;
    $account = $user;
  }

  $access = module_invoke_all('eck_access', $op, $object_type, $object, $account);

  if (!empty($access)) {
    foreach ($access as $ac) {
      $allow = $allow || $ac;
    }
  }

  return $allow;
}

/**
 * Implements hook_eck_access().
 */
function eck_eck_access($op, $object_type, $object, $account) {

  $opb = in_array($op, array('*', 'list', 'create', 'update', 'delete', 'view'));
  $otb = strcmp($object_type, "entity_type") == 0  || strcmp($object_type, "bundle") == 0
          || strcmp($object_type, "entity") == 0 || strcmp($object_type, "property") == 0;

  // Lets handle the general case of entity type permissions.
  if ($opb && $otb) {
    $master = "eck * {$object_type}";
    $perm = "eck {$op} {$object_type}";

    $allow = user_access($perm, $account) || user_access($master, $account);
    return array($allow);
  }

  return array();
}
