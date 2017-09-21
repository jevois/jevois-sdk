

inline int script_parser_fetch(char *main_name, char *sub_name, int value[], int count)
{
  script_item_value_type_e type = 0;
  script_item_u item_temp;

  type = script_get_item(main_name, sub_name, &item_temp);
  if (type == SCIRPT_ITEM_VALUE_TYPE_INT) return item_temp.val;
  else printk("WRN: failed to parse script item [%s]: '%s'\n", main_name, sub_name);
    
  return 0;
}

