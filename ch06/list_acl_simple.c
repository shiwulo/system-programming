/*
  Copyright (C) 1999-2002
  Andreas Gruenbacher, <a.gruenbacher@bestbits.at>
  shiwulo, <shiwulo@gmail.com>
  
  這個C語言程式碼的主要作用是顯示文件的訪問控制列表（ACL）條目。
  主函數遍歷ACL中的每個條目，顯示標籤類型、可選標籤限定符和權限。
  usage:./list_acl_simple ./list_acl.c
 */

// 引入必要的庫文件
#include <acl/libacl.h>
#include <stdio.h>
#include <sys/acl.h>
#include <assert.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <grp.h>
#include <attr/xattr.h>

// 主函數
int main(int argc, char *argv[])
{
    // 定義變數
    acl_t acl;
    acl_type_t aclType;
    acl_entry_t aclEntry;
    acl_tag_t aclTag;
    uid_t *uidPtr;
    gid_t *gidPtr;
    acl_permset_t permset;
    char *name;
    int entryId, permVal, opt, ret;

    // 設置ACL類型
    aclType = ACL_TYPE_ACCESS;
    if ((opt = getopt(argc, argv, "def")) != -1) {
        aclType = ACL_TYPE_DEFAULT;
    }

    // 從文件中獲取ACL
    acl = acl_get_file(argv[optind], aclType);

    // 遍歷ACL中的每個條目
    for (entryId = ACL_FIRST_ENTRY; ; entryId = ACL_NEXT_ENTRY) {
        // 檢查是否有更多條目
        if (acl_get_entry(acl, entryId, &aclEntry) != 1)
            break;

        // 獲取並顯示標籤類型
        acl_get_tag_type(aclEntry, &aclTag);
        printf("%-12s", (aclTag == ACL_USER_OBJ) ?  "owner     " :
                        (aclTag == ACL_USER) ?      "user      " :
                        (aclTag == ACL_GROUP_OBJ) ? "file grp  " :
                        (aclTag == ACL_GROUP) ?     "group     " :
                        (aclTag == ACL_MASK) ?      "mask      " :
                        (aclTag == ACL_OTHER) ?     "other     " : "???");

        // 獲取並顯示可選標籤限定符
        if (aclTag == ACL_USER) {
            uidPtr = (uid_t*) acl_get_qualifier(aclEntry);
            printf("%-8s\t", getpwuid(*uidPtr)->pw_name);
        } else if (aclTag == ACL_GROUP) {
            gidPtr = (gid_t*) acl_get_qualifier(aclEntry);
            printf("%-8s\t", getgrgid(*gidPtr)->gr_name);
        } else printf("\t\t");

        // 獲取並顯示權限
        acl_get_permset(aclEntry, &permset);
        permVal = acl_get_perm(permset, ACL_READ);
        printf("%c", (permVal == 1) ? 'r' : '-');
        permVal = acl_get_perm(permset, ACL_WRITE);
        printf("%c", (permVal == 1) ? 'w' : '-');
        permVal = acl_get_perm(permset, ACL_EXECUTE);
        printf("%c", (permVal == 1) ? 'x' : '-');
        printf("\n");
  }
// 釋放ACL資源
acl_free(acl);
}
