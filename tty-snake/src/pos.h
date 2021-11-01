#ifndef __POS_H__
#define __POS_H__

struct pos;

struct pos 
{
	int x,y;
};

struct pos pos_add(struct pos a, struct pos b)
{
	return (struct pos){ (a.x + b.x), (a.y + b.y) };
}

bool pos_cmp(struct pos a, struct pos b)
{
	return a.x == b.x && a.y == b.y;
}

#endif