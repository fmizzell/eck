/**
 * @file
 * Attaches ECK specific behaviors for configuring widgets for property
 * management through the 'Manage Fields' and 'Manage Display' pages provided
 * by the Field UI module.
 * 
 * Almost copied verbatim from field_ui.js with only tweaks to target the eck
 * specific form elements for properties.
 */
 
(function($) {

Drupal.behaviors.fieldUIFieldOverview = {
  attach: function (context, settings) {
    $('table#field-overview', context).once('field-overview', function () {
      Drupal.eckFieldUIFieldOverview.attachUpdateSelects(this, settings);
    });
  }
};

Drupal.eckFieldUIFieldOverview = {
  /**
   * Implements dependent select dropdowns on the 'Manage fields' screen.
   */
  attachUpdateSelects: function(table, settings) {
    var widgetTypes = settings.eckPropertyWidgetTypes;
    var properties = settings.eckProperties;

    // Store the default text of widget selects.
    $('.eck-widget-type-select', table).each(function () {
      this.initialValue = this.options[0].text;
    });

    // 'Property type' select updates its 'Widget' select.
    $('.eck-property-type-select', table).each(function () {
      this.targetSelect = $('.eck-widget-type-select', $(this).closest('tr'));

      $(this).bind('change keyup', function () {
        var selectedProperty = this.options[this.selectedIndex].value;
        var selectedPropertyType = (selectedProperty in properties ? properties[selectedProperty].type : null);
        var options = (selectedPropertyType in widgetTypes ? widgetTypes[selectedPropertyType] : []);
        this.targetSelect.fieldUIPopulateOptions(options);
      });

      // Trigger change on initial pageload to get the right widget options
      // when field type comes pre-selected (on failed validation).
      $(this).trigger('change', false);
    });
  }
};

})(jQuery);
