#ifndef CQCT_VM_OPTIONS_H_
#define CQCT_VM_OPTIONS_H_

#ifdef __cplusplus
extern "C" {
#endif

struct vm_options;

/**
 * Initialize default options.
 *
 * See also cqct_options_release.
 */
void cqct_options_init(struct vm_options* options);

/** Set the path to a heap file to be restored. */
void cqct_options_set_memfile(struct vm_options* options,
                              const char *memfile);

/**
 * Set the library search paths.
 *
 * @param loadpath a null-terminated array of path strings
 */
void cqct_options_set_loadpath(struct vm_options* options,
                               const char **loadpath);

struct Xfd;
void cqct_options_set_io(struct vm_options* options,
                         const Xfd* in, const Xfd* out, const Xfd* err);

/** Releases any storage associated with the options. */
void cqct_options_release(struct vm_options* options);

#ifdef __cplusplus
}  // extern "C"
#endif

#endif /* CQCT_VM_OPTIONS_H_ */
