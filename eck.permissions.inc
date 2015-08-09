<?php
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
 * Implements hook_permission().
 *
 * A pretty through set of permission
 * you can set permissiona at each level: entity_type, bundle, entity
 * and for each action of the CRUD
 */
function eck_permission() {

  module_load_include('inc', 'eck', 'eck.entity_type');
  module_load_include('inc', 'eck', 'eck.bundle');

  $perms = array(
    // Entity Type permissions.
    'eck administer entity types' => array(
      'title' => t('Administer Entity Types'),
      'restrict access' => TRUE,
      'description' => t('Grants the ability to administer(add/edit/delete/view) any ECK Entity Type.'),
    ),
    'eck add entity types' => array(
      'title' => t('Add Entity Types'),
      'restrict access' => TRUE,
    ),
    'eck edit entity types' => array(
      'title' => t('Edit Entity Types'),
      'restrict access' => TRUE,
    ),
    'eck delete entity types' => array(
      'title' => t('Delete Entity Types'),
      'restrict access' => TRUE,
    ),
    'eck list entity types' => array(
      'title' => t('View Entity Type List'),
    ),
    // Bundle Permissions.
    'eck administer bundles' => array(
      'title' => t('Administer Bundles'),
      'restrict access' => TRUE,
      'description' => t('Grants the ability to administer(add/edit/delete/list) any ECK Bundle.'),
    ),
    'eck add bundles' => array(
      'title' => t('Add Bundles'),
      'restrict access' => TRUE,
      'description' => t('Grants the ability to add new bundles to any Entity type.'),
    ),
    'eck edit bundles' => array(
      'title' => t('Edit Bundles'),
      'restrict access' => TRUE,
    ),
    'eck delete bundles' => array(
      'title' => t('Delete Bundles'),
      'restrict access' => TRUE,
    ),
    'eck list bundles' => array(
      'title' => t('View Bundle Lists'),
    ),
    // Entitiy Permissions.
    'eck administer entities' => array(
      'title' => t('Administer Entities'),
      'restrict access' => TRUE,
    ),
    'eck add entities' => array(
      'title' => t('Add Entities'),
      'restrict access' => TRUE,
      'description' => t('Grants the ability to add new entities of any Entity type.'),
    ),
    'eck edit entities' => array(
      'title' => t('Edit Any Entity'),
      'restrict access' => TRUE,
    ),
    'eck delete entities' => array(
      'title' => t('Delete Any Entity'),
      'restrict access' => TRUE,
    ),
    'eck view entities' => array(
      'title' => t('View Any Entity'),
    ),
    'eck list entities' => array(
      'title' => t('View Entity Lists'),
      'description' => t('Grants the ability to view the list of entities displayed on any bundle page.'),
    ),
  );

  $perms_info = array(
    'eck administer' => 'Administer',
    'eck add' => "Add",
    'eck edit' => 'Edit',
    'eck delete' => 'Delete',
    'eck list' => 'View List of',
    'eck view' => 'View',
  );

  foreach (EntityType::loadAll() as $entity_type) {

    $perms["manage {$entity_type->name} properties"] = array(
      'title' => "Manage {$entity_type->label} Properties",
    );

    foreach ($perms_info as $op => $op_label) {
      $perms["{$op} {$entity_type->name} bundles"] = array(
        'title' => "{$op_label} {$entity_type->label} Bundles",
      );
    }

    foreach (Bundle::loadByEntityType($entity_type) as $bundle) {
      $entity_perm_info = $perms_info + array('eck view' => "View");

      foreach ($entity_perm_info as $op => $op_label) {
        $perm_name = "{$op} {$entity_type->name} {$bundle->name} entities";
        $perms[$perm_name] = array(
          'title' => "{$op_label} {$entity_type->label} {$bundle->label} Entities",
        );
      }
    }
  }

  return $perms;
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
