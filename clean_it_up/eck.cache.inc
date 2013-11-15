<?php

/**
 * Implements hook_flush_caches().
 */

function eck_flush_caches() {
  // Reload all property types.
  eck_get_property_types(NULL, TRUE);
  // Reload all property widget types.
  eck_property_info_widget_types(NULL, TRUE);
  // Reload all ECK entity types.
  EntityType::loadAll(NULL, TRUE);
  // Reload all Bundles.
  Bundle::loadAll(NULL, TRUE);
}
