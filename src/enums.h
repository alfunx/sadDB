#ifndef SADDB_ENUMS_H_
#define SADDB_ENUMS_H_

enum relation_type
{
	R,
	S
};

relation_type other_relation_type(relation_type type)
{
	return (relation_type)((type + 1) % 2);
}

#endif // SADDB_ENUMS_H_
