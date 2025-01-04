#include"FaithItem.hxx"


int ITK_user_main(int 	argc, char* argv[]) {
	printf("Utility started...\n");

	tag_t type = NULLTAG;
	tag_t create_input = NULLTAG;
	tag_t NewItem = NULLTAG;
	tag_t userTag = NULLTAG;
	tag_t Ufolder = NULLTAG;
	tag_t uom = NULLTAG;
	tag_t DStype = NULLTAG;
	tag_t DScreate_input = NULLTAG;
	tag_t NewDS = NULLTAG;

	// variable
	char* item_id = NULL;
	char* item_name = NULL; 
	int status = ITK_ok;

	char* user = ITK_ask_cli_argument("-u=");
	char* password = ITK_ask_cli_argument("-p=");
	char* group = ITK_ask_cli_argument("-g=");

	status = ITK_init_module(user, password, group);
	if (status == ITK_ok) {
		printf("Login Successful.\n");
	}
	else {
		printf("Failed to login.\n");
	}

	
	status = TCTYPE_ask_type("A3BHMakePart", &type);		
	status = TCTYPE_construct_create_input(type, &create_input);
	status = AOM_set_value_string(create_input, "object_name", "Faith Item");
	status = AOM_set_value_string(create_input, "uom_tag", "Each");
	status = AOM_set_value_string(create_input, "a3MakeBuy", "Make");
	status = TCTYPE_create_object(create_input,&NewItem);
	status = AOM_save_with_extensions(NewItem);
	

	status = TCTYPE_ask_type("Dataset", &DStype);
	status = TCTYPE_construct_create_input(type, &DScreate_input);
	status = AOM_set_value_string(create_input, "object_name", "Faith Dataset");	
	status = TCTYPE_create_object(create_input, &NewDS);
	status = AOM_save_with_extensions(NewDS);

	status = SA_find_user2(user, &userTag);
	status = SA_ask_user_home_folder(userTag, &Ufolder);
	status = FL_insert(Ufolder, NewItem, 999);
	status = AOM_save_with_extensions(Ufolder);




	/* printing newly created item	
	char* UOMvalue = NULL;
	status = ITEM_ask_id2(NewItem, &item_id);
	status = ITEM_ask_name2(0NewItem, &item_name);
	status = ITEM_ask_unit_of_measure(NewItem, &uom);
	
	printf("Item ID\t\tItem Name\t\tUOM\n");
	printf("%s\t%s\t%d", item_id, item_name, uom);*/

	status = ITK_exit_module(true);
	if (status == ITK_ok) {
		printf("\nLogout Successful.\n");
	}
	else {
		printf("\nFailed to logout.\n");
	}

	(void)_getch();
	return status;
}