<?php
/**
 * @file
 * Field handler to present a link to delete the entity content.
 */

// @codingStandardsIgnoreStart
class eck_views_handler_field_link_delete extends eck_views_handler_field_link {

  function render_link($entity, $values) {
    $entity_type = $entity->entityType();
    $bundle = $entity->bundle();
    $action = 'delete';

    if (eck_access($action, 'entity', $entity)) {
      $crud_info = get_bundle_crud_info($entity_type, $bundle);
      $this->options['alter']['make_link'] = TRUE;
      $this->options['alter']['path'] = str_replace('%eckentity', $entity->id, $crud_info['delete']['path']);
      $this->options['alter']['query'] = drupal_get_destination();
      $text = !empty($this->options['text']) ? $this->options['text'] : t('delete');

      return $text;
    }
  }
}
// @codingStandardsIgnoreEnd
