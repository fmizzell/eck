<?php

/**
 * Implements hook_views_api().
 */
function eck_views_api() {
  return array(
    'api'   => '3',
    'path'  => drupal_get_path('module', 'eck') . '/views',
  );
}
